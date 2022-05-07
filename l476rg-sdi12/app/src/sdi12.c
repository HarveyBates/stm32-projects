#include "sdi12.h"

SDI12_TypeDef sdi12;

/* Private member functions */
static HAL_StatusTypeDef SDI12_QueryDevice(char *cmd, uint8_t cmd_len,
		char *response, uint8_t response_len);

/*
 * Initialise with UART, TX Pin and TX Pin GPIO Port.
 */
void SDI12_Init(UART_HandleTypeDef *huart) {
	sdi12.Huart = huart;
	sdi12.Pin = SDI12_COM_Pin;
	sdi12.Port = SDI12_COM_GPIO_Port;
}

/*
 * Main function to deal with SDI12 commands and responses.
 */
static HAL_StatusTypeDef SDI12_QueryDevice(
		char *cmd,
		uint8_t cmd_len,
		char *response,
		uint8_t response_len) {

	// Setup GPIO pin for break
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = sdi12.Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(sdi12.Port, &GPIO_InitStruct);

	// Break must be >= 12 ms
	HAL_GPIO_WritePin(sdi12.Port, (uint16_t)sdi12.Pin, GPIO_PIN_SET);
	HAL_Delay(12);

	// Marking must be >= 8.3 ms
	HAL_GPIO_WritePin(sdi12.Port, (uint16_t)sdi12.Pin, GPIO_PIN_RESET);
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(sdi12.Port, &GPIO_InitStruct);
	HAL_Delay(8);

	// Transmit
	HAL_StatusTypeDef res = HAL_UART_Transmit(sdi12.Huart, (uint8_t *)cmd, cmd_len, 1000);
	if(res != HAL_OK) {
		return res;
	}

	// Enable swap UART
	sdi12.Huart->AdvancedInit.Swap = UART_ADVFEATURE_SWAP_ENABLE;
	res = HAL_UART_Init(sdi12.Huart);
	if(res != HAL_OK) {
		return res;
	}

	__HAL_UART_FLUSH_DRREGISTER(sdi12.Huart);

	// Recieve
	res = HAL_UART_Receive(sdi12.Huart, (uint8_t *)response, response_len, 380);
	if(res != HAL_TIMEOUT) {
		return res;
	}

	// Disable swap UART
	sdi12.Huart->AdvancedInit.Swap = UART_ADVFEATURE_SWAP_DISABLE;
	res = HAL_UART_Init(sdi12.Huart);
	if(res != HAL_OK) {
		return res;
	}

	__HAL_UART_FLUSH_DRREGISTER(sdi12.Huart);

	return HAL_OK;
}


/*
 * Simple SDI12 command to determine if a device is active on the queried address.
 * Expected response {'0', '\r', '\n'} where '0' is the address.
 */
HAL_StatusTypeDef SDI12_AckActive(char *addr) {
	char cmd[3] = {*addr, '!', 0x00};
	char response[3] = {0};
	HAL_StatusTypeDef result = SDI12_QueryDevice(cmd, 3, response, 3);
	return result;
}

/*
 * Used to populate a list of connected device addresses.
 */
void SDI12_DevicesOnBus(uint8_t* devices) {
	uint8_t index = 0;
	for(char i = '0'; i < '9'; i++) {
		HAL_StatusTypeDef result = SDI12_AckActive(&i);
		if(result == HAL_OK) {
			devices[index++] = i;
		}
	}
}

/*
 * Change a devices SDI12 address.
 * May not work on all devices. Only those who support this feature.
 * Expected response {'1', '\r', '\n'} where '1' is the new address
 */
HAL_StatusTypeDef SDI12_ChangeAddr(char *from_addr, char *to_addr) {
	char cmd[5] = {*from_addr, 'A', *to_addr, '!', 0x00};
	char response[3] = {0};
	HAL_StatusTypeDef result = SDI12_QueryDevice(cmd, 5, response, 100);
	return result;
}


/*
 * Start measurement. No CRC.
 * Expected response {'0', '0', '0', '1', '3', '\r', '\n'}
 * Expected response as = atttn -> address (a), 3 time numbers (t) and n results (n).
 */
HAL_StatusTypeDef SDI12_StartMeasurement(char *addr, SDI12_Measure_TypeDef *measurement_info) {
	char cmd[4] = {*addr, 'M', '!', 0x00};
	char response[7] = {0};
	HAL_StatusTypeDef result = SDI12_QueryDevice(cmd, 4, response, 7);

	// Check for valid response
	if (response[0] != '\0') {
		// Address of queried device (a)
		measurement_info->Address = (uint8_t)response[0];

		// Extract time from response to be converted to uint16_t
		char time_buf[3];
		for(uint8_t i = 1; i < 4; i++) {
			time_buf[i-1] = response[i];
		}

		// Convert time_buf (ttt) into uint16_t
		uint16_t time;
		measurement_info->Time = sscanf(time_buf, "%hd", &time);

		// Number of values to expect in measurement (n)
		measurement_info->NumValues = response[4];
	};

	return result;
}

