#include "main.h"
#include "stdio.h"
#include "string.h"

/* Private variables */
I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart2;

/* I2C Address for AHT25 */
#define AHT25_ADDR 0x70

/* AHT25 Commands */
#define AHT25_INIT_CMD 0xE1
#define AHT25_MEASURE_CMD 0xAC
#define AHT25_SOFT_RESET 0xFE

/* Function prototypes */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
void AHT25_Init(void);
void Read_Sensor_Values(float *temperature, float *humidity);

int main(void)
{
    /* Initialize the HAL Library */
    HAL_Init();
    /* Configure the system clock */
    SystemClock_Config();
    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_USART2_UART_Init();

    /* Initialize the AHT25 sensor */
    AHT25_Init();

    /* Variable to hold sensor data */
    float temperature, humidity;

    while (1)
    {
        /* Read sensor values */
        Read_Sensor_Values(&temperature, &humidity);

        /* Transmit humidity */
        char buffer[128];
        sprintf(buffer, "Humidity: %.2f%%\r\n", humidity);
        HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);

        /* Transmit temperature */
        sprintf(buffer, "Temperature: %.2f°C\r\n", temperature);
        HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);

        /* Delay before the next reading */
        HAL_Delay(100);
    }
}

void AHT25_Init(void)
{
    uint8_t initCmd[3] = { AHT25_INIT_CMD, 0x08, 0x00 }; // Initialization command
    HAL_I2C_Master_Transmit(&hi2c1, AHT25_ADDR << 1, initCmd, sizeof(initCmd), HAL_MAX_DELAY);
    HAL_Delay(50); // Wait for initialization to complete

    uint8_t measureCmd[3] = { AHT25_MEASURE_CMD, 0x00, 0x00 }; // Command to start measurement
    HAL_I2C_Master_Transmit(&hi2c1, AHT25_ADDR << 1, measureCmd, sizeof(measureCmd), HAL_MAX_DELAY);
    HAL_Delay(100); // Wait for the measurement to complete
}

void Read_Sensor_Values(float *temperature, float *humidity)
{
	uint8_t data[6];
	uint8_t cmd = AHT25_MEASURE_CMD;
	HAL_I2C_Master_Transmit(&hi2c1, AHT25_ADDR,&cmd, 1, HAL_MAX_DELAY);
	HAL_Delay(100);
	HAL_I2C_Master_Receive(&hi2c1,0x71, data, 6, HAL_MAX_DELAY);
	*humidity = ((float)((data[1] << 12) | (data[2] << 4) | (data[3] >> 4))) /1048576.0 * 100.0;
	*temperature = ((float)(((data[3] & 0x0F) << 16) | (data[4] << 8) |data[5])) / 1048576.0 * 200.0 - 50.0;
}


/* I2C1 Initialization Function */
static void MX_I2C1_Init(void)
{
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        Error_Handler();
    }
}

/* USART2 Initialization Function */
static void MX_USART2_UART_Init(void)
{
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
}

/* GPIO Initialization Function */
static void MX_GPIO_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
}

void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }
}
