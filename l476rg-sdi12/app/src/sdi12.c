#include "sdi12.h"

SDI12_TypeDef sdi12;

void SDI12_Init(UART_HandleTypeDef *huart) {
	sdi12.huart = huart;
}

/*
 * Generic function to get information from sensors on the bus
 */
int SDI12_GetDeviceId(uint8_t *addr) {

	// Setup GPIO pin for break
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = SDI12_COM_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(SDI12_COM_GPIO_Port, &GPIO_InitStruct);

	// Break cmd must be >= 12 ms
	HAL_GPIO_WritePin(SDI12_COM_GPIO_Port, SDI12_COM_Pin, GPIO_PIN_SET);
	HAL_Delay(12);

	// Marking must be >= 8.3 ms
	HAL_GPIO_WritePin(SDI12_COM_GPIO_Port, SDI12_COM_Pin, GPIO_PIN_RESET);
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(SDI12_COM_GPIO_Port, &GPIO_InitStruct);
	HAL_Delay(8);

	// Transmit
	uint8_t data[] = {*addr, 'I', '!', 0x00};
	HAL_StatusTypeDef res = HAL_UART_Transmit(sdi12.huart, data, sizeof(data), 1000);
	if(res != HAL_OK) {
		return res;
	}

	// Enable swap UART
	sdi12.huart->AdvancedInit.Swap = UART_ADVFEATURE_SWAP_ENABLE;
	res = HAL_UART_Init(sdi12.huart);
	if(res != HAL_OK) {
		return res;
	}

	__HAL_UART_FLUSH_DRREGISTER(sdi12.huart);

	// Recieve
	uint8_t buffer[256] = {0};
	res = HAL_UART_Receive(sdi12.huart, buffer, 256, 380);
	if(res != HAL_TIMEOUT) {
		return res;
	}

	// Disable swap UART
	sdi12.huart->AdvancedInit.Swap = UART_ADVFEATURE_SWAP_DISABLE;
	res = HAL_UART_Init(sdi12.huart);
	if(res != HAL_OK) {
		return res;
	}

	__HAL_UART_FLUSH_DRREGISTER(sdi12.huart);

	return 0;
}
