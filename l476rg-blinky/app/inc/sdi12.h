#ifndef SDI12_
#define SDI12_

#include <stdio.h>
#include <string.h>

#include "main.h"

typedef struct {
		UART_HandleTypeDef huart;
		GPIO_TypeDef *GPIO_Pin_Port;
		uint16_t GPIO_Pin;
} SDI12_TypeDef;

void SDI12_Init(UART_HandleTypeDef huart);
void SDI12_GetDeviceInfo();
int SDI12_CmdWithResponse(char *cmd);

#endif // SDI12_
