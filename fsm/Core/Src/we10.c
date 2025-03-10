#include "we10.h"
#include <string.h>
#include <stdio.h>

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

void WE10_reset(void) {
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "CMD+RESET\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 1000);
    HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), 1000);

    HAL_Delay(2000); // Allow time for reset

    // Clear buffer before receiving
    memset(buffer, 0, sizeof(buffer));
    HAL_UART_Receive(&huart1, (uint8_t*)buffer, sizeof(buffer), 1000);
}

void WE10_wifimode(void) {
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "CMD+WIFIMODE=1\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 1000);
    HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), 1000);

    // Wait for the response
    HAL_Delay(2000);
    memset(buffer, 0, sizeof(buffer));
}

void WE10_contoap(void) {
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "CMD+CONTOAP=VIVO,12345678\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 1000);
    HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), 1000);

    HAL_Delay(5000);
    memset(buffer, 0, sizeof(buffer));

}

void MQTT_netcfg(void) {
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "CMD+MQTTNETCFG=dev.rightech.io,1883\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 1000);
    HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), 1000);
    HAL_Delay(500); // Short delay for processing
    memset(buffer, 0, sizeof(buffer));
    HAL_UART_Receive(&huart1, (uint8_t*)buffer, sizeof(buffer), 1000);

    snprintf(buffer, sizeof(buffer), "CMD+MQTTCONCFG=3,mqtt-saikumarsunny2002-3ruesz,,,,,,,,,\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 1000);
    HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), 1000);
    HAL_Delay(500); // Short delay for processing
    memset(buffer, 0, sizeof(buffer));
    HAL_UART_Receive(&huart1, (uint8_t*)buffer, sizeof(buffer), 1000);
}

void MQTT_start(void) {
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "CMD+MQTTSTART=1\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 1000);
    HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), 1000);
    HAL_Delay(5000); // Allow time for starting MQTT
    memset(buffer, 0, sizeof(buffer));
    HAL_UART_Receive(&huart1, (uint8_t*)buffer, sizeof(buffer), 1000);
}

void WE10_RestartInitialization(void) {
    WE10_reset();
    WE10_wifimode();
    WE10_contoap();
    MQTT_netcfg();
    MQTT_start();
}
