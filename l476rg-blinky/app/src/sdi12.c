#include "sdi12.h"

SDI12_TypeDef sdi12;

void SDI12_Init(UART_HandleTypeDef huart) {
	sdi12.huart = huart;
}

/*
 * Generic function to get information from sensors on the bus
 */
void SDI12_GetDeviceInfo() {
	char cmd[] = "I!";
	SDI12_CmdWithResponse(cmd);
}

/*
 * Send command with response from device (generic function)
 */
int SDI12_CmdWithResponse(char *cmd) {

	// TODO Check if valid command

	HAL_GPIO_WritePin(GPIOA, SDI12_COM_Pin, GPIO_PIN_SET);

	if(HAL_LIN_SendBreak(&sdi12.huart) != HAL_OK) {
		return 1;
	}

	HAL_Delay(20);

	uint8_t data[] = "I!";
	if(HAL_UART_Transmit(&sdi12.huart, (uint8_t *)data, sizeof(data), 1000) != HAL_OK) {
		return 1;
	}

	__HAL_UART_FLUSH_DRREGISTER(&sdi12.huart);

	uint8_t buffer[256];
	memset(buffer, 0, sizeof(buffer));
	if(HAL_UART_Receive(&sdi12.huart, buffer, sizeof(buffer), 100) != HAL_OK) {
		return 1;
	}

	return 0;

}
