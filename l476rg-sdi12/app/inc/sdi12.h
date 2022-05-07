#ifndef SDI12_
#define SDI12_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "main.h"

#define MAX_RESPONSE_SIZE 75

/*
 * GPIO Pin, Port and UART for SDI12 functions.
 */
typedef struct {
	UART_HandleTypeDef *Huart;
	uint32_t Pin;
	GPIO_TypeDef *Port;
} SDI12_TypeDef;

/*
 * Measurement query response.
 * i.e. Query = aM!
 * Response = atttn where a = address, t = three time numbers and n is the
 * expected number of values.
 */
typedef struct {
	char Address;
	uint16_t Time;
	uint8_t NumValues;
} SDI12_Measure_TypeDef;

void SDI12_Init(UART_HandleTypeDef *huart);
HAL_StatusTypeDef SDI12_AckActive(char *addr);
void SDI12_DevicesOnBus(uint8_t* devices);
HAL_StatusTypeDef SDI12_ChangeAddr(char *from_addr, char *to_addr);
HAL_StatusTypeDef SDI12_StartMeasurement(char *addr, SDI12_Measure_TypeDef *measure_info);
HAL_StatusTypeDef SDI12_SendData(char *addr, SDI12_Measure_TypeDef *measurement_info, char *data);

#endif // SDI12_
