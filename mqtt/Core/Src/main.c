/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : FSM-based Main program body
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#define AHT25_I2C_ADDR (0x38 << 1)   // AHT25 I2C address
#define PUB_DELAY 5000               // Publish every 5 seconds
#define WIFI_CONNECTION_TIMEOUT 20000 // 20 seconds timeout

typedef enum {
    STATE_INITIALIZE,
    STATE_CONNECT_WIFI,
    STATE_READ_SENSOR,
    STATE_PUBLISH_MQTT,
    STATE_ERROR
} FSM_State;

FSM_State currentState = STATE_INITIALIZE;

// Wi-Fi Configuration Structure
typedef struct {
    const char *ssid;
    const char *password;
    const char *mqtt_server;
    const char *mqtt_client_id;
} WiFiConfig;

// Initialize Wi-Fi Configuration
WiFiConfig wifi_config = {
    .ssid = "VIVO",
    .password = "12345678",
    .mqtt_server = "dev.rightech.io",
    .mqtt_client_id = "saikumarsunny2002-3ruesz"
};

I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart1;

void SystemClock_Config(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_GPIO_Init(void);
void Error_Handler(void);
void AHT25_Init(void);
void AHT25_ReadData(float *temperature, float *humidity);
void WE10_Init(void);
void MQTT_PublishTemperature(float temperature);
void MQTT_PublishHumidity(float humidity);
void WiFi_SendCommand(char *command, char *response, uint16_t response_len);
int WiFi_Connected(void);
void debug_print(const char *format, ...);

uint32_t last_publish_time = 0;
uint32_t connection_start_time = 0;

float temperature = 0.0f;
float humidity = 0.0f;

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART1_UART_Init();
    MX_I2C1_Init();

    // FSM Main Loop
    while (1) {
        switch (currentState) {
            case STATE_INITIALIZE:
                AHT25_Init();           // Initialize AHT25 sensor
                WE10_Init();            // Initialize Wi-Fi and MQTT
                connection_start_time = HAL_GetTick(); // Start connection timer
                currentState = STATE_CONNECT_WIFI; // Transition to Wi-Fi connection state
                break;

            case STATE_CONNECT_WIFI:
                if (WiFi_Connected()) {  // Check if Wi-Fi is successfully connected
                    currentState = STATE_READ_SENSOR;
                } else if (HAL_GetTick() - connection_start_time > WIFI_CONNECTION_TIMEOUT) {
                    currentState = STATE_ERROR; // Go to error state if connection timeout
                }
                break;

            case STATE_READ_SENSOR:
                AHT25_ReadData(&temperature, &humidity); // Read temperature and humidity
                debug_print("Temperature: %.2f, Humidity: %.2f\n", temperature, humidity); // Log sensor data
                currentState = STATE_PUBLISH_MQTT;       // Transition to publishing state
                break;

            case STATE_PUBLISH_MQTT:
                if (HAL_GetTick() - last_publish_time > PUB_DELAY) {
                    MQTT_PublishTemperature(temperature); // Publish temperature
                    MQTT_PublishHumidity(humidity);       // Publish humidity
                    last_publish_time = HAL_GetTick();    // Reset publish timer
                }
                currentState = STATE_READ_SENSOR;  // Go back to reading sensor data
                break;

            case STATE_ERROR:
                // Handle error, reset device or perform recovery actions
                Error_Handler();
                break;

            default:
                currentState = STATE_ERROR;  // Set to error state if undefined state occurs
                break;
        }
        HAL_Delay(100); // Delay for basic task scheduling
    }
}

// Function Definitions

// Debug Print Function
void debug_print(const char *format, ...) {
    char buffer[256]; // Adjust size as needed
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    for (char *p = buffer; *p; p++) {
        ITM_SendChar(*p); // Send each character to SWV
    }
}

// AHT25 Initialization Function
void AHT25_Init(void) {
    uint8_t cmd[3] = {0xAC, 0x33, 0x00};
    if (HAL_I2C_Master_Transmit(&hi2c1, AHT25_I2C_ADDR, cmd, sizeof(cmd), 1000) != HAL_OK) {
        currentState = STATE_ERROR;
    }
    HAL_Delay(500);
}

// AHT25 Data Read Function
void AHT25_ReadData(float *temperature, float *humidity) {
    uint8_t data[6];
    uint8_t cmd = 0xAC;
    if (HAL_I2C_Master_Transmit(&hi2c1, AHT25_I2C_ADDR, &cmd, 1, 1000) != HAL_OK) {
        currentState = STATE_ERROR;
        return;
    }
    HAL_Delay(100);
    if (HAL_I2C_Master_Receive(&hi2c1, AHT25_I2C_ADDR, data, sizeof(data), 1000) != HAL_OK) {
        currentState = STATE_ERROR;
        return;
    }

    uint32_t raw_temp = ((data[3] & 0x0F) << 16) | (data[4] << 8) | data[5];
    uint32_t raw_humid = ((data[1] << 16) | (data[2] << 8) | data[3]) >> 4;

    *temperature = ((float)raw_temp / 1048576) * 200.0f - 50.0f;
    *humidity = ((float)raw_humid / 1048576) * 100.0f;
}

// WE10 Wi-Fi Initialization Function
void WE10_Init(void) {
    char response[100];

    // Reset the module
    WiFi_SendCommand("CMD+RESET\r\n", response, sizeof(response));
    HAL_Delay(1000);

    // Set Wi-Fi Mode to Station
    WiFi_SendCommand("CMD+WIFIMODE=1\r\n", response, sizeof(response));
    HAL_Delay(500);

    // Connect to Access Point (Wi-Fi)
    char command[150];
    snprintf(command, sizeof(command), "CMD+CONTOAP=%s,%s\r\n", wifi_config.ssid, wifi_config.password);
    WiFi_SendCommand(command, response, sizeof(response));
    HAL_Delay(2000);

    // Set MQTT connection configuration
    snprintf(command, sizeof(command), "CMD+MQTTNETCFG=%s,1883\r\n", wifi_config.mqtt_server);
    WiFi_SendCommand(command, response, sizeof(response));
    HAL_Delay(500);

    // Set MQTT client details
    snprintf(command, sizeof(command), "CMD+MQTTCONCFG=3,%s,,,,,,,,,,\r\n", wifi_config.mqtt_client_id);
    WiFi_SendCommand(command, response, sizeof(response));
    HAL_Delay(500);

    // Start MQTT
    WiFi_SendCommand("CMD+MQTTSTART=1\r\n", response, sizeof(response));
    HAL_Delay(5000);
}

// MQTT Publish Temperature
void MQTT_PublishTemperature(float temperature) {
    char command[50];
    snprintf(command, sizeof(command), "CMD+MQTTPUB=base/state/temperature,%.2f\r\n", temperature);
    WiFi_SendCommand(command, NULL, 0);  // Publish without waiting for response
}

// MQTT Publish Humidity
void MQTT_PublishHumidity(float humidity) {
    char command[50];
    snprintf(command, sizeof(command), "CMD+MQTTPUB=base/state/humidity,%.2f\r\n", humidity);
    WiFi_SendCommand(command, NULL, 0);  // Publish without waiting for response
}

// Send a command to WE10 module via UART
void WiFi_SendCommand(char *command, char *response, uint16_t response_len) {
    HAL_UART_Transmit(&huart1, (uint8_t *)command, strlen(command), 1000);
    if (response != NULL) {
        HAL_UART_Receive(&huart1, (uint8_t *)response, response_len, 1000);
    }
}

// Function to Check Wi-Fi Connection
int WiFi_Connected(void) {
    char response[100];
    WiFi_SendCommand("CMD+STATUS\r\n", response, sizeof(response));
    // Check for "Connected" in the response
    return strstr(response, "Connected") != NULL;
}

// System Clock Configuration
// System Clock Configuration
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);
}

// I2C1 Initialization Function
static void MX_I2C1_Init(void) {
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    HAL_I2C_Init(&hi2c1);
}

// USART1 Initialization Function
static void MX_USART1_UART_Init(void) {
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huart1);
}

// GPIO Initialization Function
static void MX_GPIO_Init(void) {
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
}

// Error Handler Function
void Error_Handler(void) {
    while (1) {
        // Stay here in case of an error
    }
}
