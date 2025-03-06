#include "stm32f4xx_hal.h" // Change according to your STM32 series
#include <string.h>
#include <stdio.h>

typedef enum {
    IDLE,
    CONNECTING,
    CONNECTED,
    DISCONNECTED
} WiFiState;

WiFiState currentState = IDLE;
UART_HandleTypeDef huart1;

#define BUFFER_SIZE 100
char rxBuffer[BUFFER_SIZE];
volatile int rxIndex = 0;

void WiFi_Init(void);
void WE10_Init(char *SSID, char *PASSWD);
void WiFi_Update(void);
void WiFi_HandleEvent(char* event);
void UART_RxCallback(void);
void Error_Handler(void);

int main(void) {
    HAL_Init();
    WiFi_Init();

    // Initialize with your SSID and Password
    WE10_Init("VIVO", "12345678");

    while (1) {
        WiFi_Update();
        HAL_Delay(100);
    }
}

void WiFi_Init(void) {
    __HAL_RCC_USART1_CLK_ENABLE();
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart1) != HAL_OK) {
        Error_Handler();
    }

    HAL_UART_Receive_IT(&huart1, (uint8_t *)&rxBuffer[rxIndex], 1);
}

void WE10_Init(char *SSID, char *PASSWD) {
    char buffer[128];

    /********* CMD+RESET **********/
    sprintf(buffer, "CMD+RESET\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 1000);
    HAL_Delay(500); // Give time for the command to process

    /********* CMD+WIFIMODE=1 **********/
    sprintf(buffer, "CMD+WIFIMODE=1\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 1000);
    HAL_Delay(2000); // Wait for the command to process

    /********* CMD+CONTOAP=SSID,PASSWD **********/
    sprintf(buffer, "CMD+CONTOAP=%s,%s\r\n", SSID, PASSWD);
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 1000);
    HAL_Delay(2000); // Allow time for connection attempt

    /********* CMD?WIFI **********/
    sprintf(buffer, "CMD?WIFI\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 1000);
    HAL_Delay(500); // Wait before receiving the response

    HAL_UART_Receive(&huart1, (uint8_t*)buffer, sizeof(buffer), 1000);
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 1000); // Debugging output
}

void WiFi_Update(void) {
    if (rxIndex > 0) {
        rxBuffer[rxIndex] = '\0'; // Null-terminate the string
        printf("Received: %s\n", rxBuffer); // Debug output
        WiFi_HandleEvent(rxBuffer);
        rxIndex = 0; // Reset index for next message
    }
}

void WiFi_HandleEvent(char* event) {
    if (strstr(event, "CONNECTED") != NULL) {
        currentState = CONNECTED;
    } else if (strstr(event, "DISCONNECTED") != NULL) {
        currentState = DISCONNECTED;
    } else if (strstr(event, "ERROR") != NULL) {
        currentState = DISCONNECTED;
        // Optionally handle reconnection logic
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        if (rxIndex < BUFFER_SIZE - 1) {
            if (rxBuffer[rxIndex] == '\n') { // Check for end of line
                WiFi_Update(); // Process the complete message
            } else {
                rxIndex++;
            }
        } else {
            // Buffer overflow handling
            rxIndex = 0;
        }
        HAL_UART_Receive_IT(&huart1, (uint8_t *)&rxBuffer[rxIndex], 1);
    }
}

void Error_Handler(void) {
    // Add LED indication or UART message
    while (1) {
        // Stay here to indicate an error
    }
}

