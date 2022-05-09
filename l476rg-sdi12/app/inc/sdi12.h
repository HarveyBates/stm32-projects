/*
 ******************************************************************************
 * @file           : sdi12.h
 * @brief          : SDI-12 library for STM32 microcontrollers.
 * 						Built using a STM32L476RG.
 ******************************************************************************
 * @attention
 *
 * !! SDI-12 uses 5 V logic, ensure your microcontrollers pins are 5 V
 * compatible !!
 *
 ******************************************************************************
 * @currently_supports
 * 	- Acknowledge active (a!)
 * 	- Send idenfification (aI!)
 * 	- Change address (aAb!)
 * 	- Start measurement (aM!)
 * 	- Send data (aD0!)
 * 	- Start verification (aV!)
 ******************************************************************************
 */

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
 * Query = aM!
 * Response = atttn where a = address, t = three numbers indication response
 * time (ttt) and the expected number of values (n)
 */
typedef struct {
	char Address;
	uint16_t Time;
	uint8_t NumValues;
} SDI12_Measure_TypeDef;

/*
 * Verification information response.
 * Query = aV!
 * Response = atttn where a = address, t = three numbers indication response
 * time (ttt) and the expected number of values (n)
 */
typedef struct {
	char Address;
	uint16_t Time;
	uint8_t NumValues;
} SDI12_Verification_TypeDef;

void SDI12_Init(UART_HandleTypeDef *huart);
HAL_StatusTypeDef SDI12_AckActive(char *addr);
void SDI12_DevicesOnBus(char* devices);
HAL_StatusTypeDef SDI12_ChangeAddr(char *from_addr, char *to_addr);
HAL_StatusTypeDef SDI12_StartMeasurement(char *addr, SDI12_Measure_TypeDef *measure_info);
HAL_StatusTypeDef SDI12_SendData(char *addr, SDI12_Measure_TypeDef *measurement_info, char *data);

#endif // SDI12_
