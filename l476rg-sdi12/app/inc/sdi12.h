#ifndef SDI12_
#define SDI12_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "main.h"

/*
 * List of generic SDI12 commands.
 * Need to change address when using.
 */
// Generic request for single device on bus
uint8_t QUERY_ADDR_SINGLE[] = {'?', '!', 0x00}; // ?!

// Requires address to be known (or searched for)
uint8_t ACK_ACTIVE[] = {0x00, '!', 0x00}; // a!
uint8_t REQUEST_ID[] = {0x00, 'I', '!', 0x00}; //aI!
uint8_t CHANGE_ADDR[] = {0x00, 'A', 'b', 0x00, '!', 0x00}; // aAb!
uint8_t SEND_DATA[] = {0x00, 'D', '!', 0x00}; // aD0! -> aD9!
uint8_t START_CONCURRENT_MEASUREMENT[] = {0x00, 'C', '!', 0x00}; // aC!!
uint8_t ADDITIONAL_CONCURRENT_MEASUREMENT[] = {0x00, 'C', 0x00, '!', 0x00}; // aC1! -> aC9!
uint8_t CONTINUOUS_MEASUREMENT[] = {0x00, 'R', 0x00, '!', 0x00}; // aR0! -> aR9!

// May result in service request see section 4.4.6 in SDI12 spec
uint8_t START_VERIFICATION[] = {0x00, 'V', '!', 0x00}; // aV!
uint8_t START_MESUREMENT[] = {0x00, 'M', '!', 0x00}; // aM!
uint8_t ADDITIONAL_MEASUREMENT[] = {0x00, 'M', 0x00, '!', 0x00}; // aM1! -> aM9!

// Greater than SDI12 spec 1.3
uint8_t START_MEASUREMENT_CRC[] = {0x00, 'M', 'C', '!', 0x00}; // aMC!
uint8_t ADDITIONAL_MEASUREMENT_CRC[] = {0x00, 'M', 0x00, '!', 0x00}; // aMC1! -> aMC9!
uint8_t START_CONCURRENT_MEASUREMENT_CRC[] = {0x00, 'C', 'C', '!', 0x00}; // aCC!
uint8_t ADDITIONAL_CONCURRENT_MEASUREMENT_CRC[] = {0x00, 'C', 'C', 0x00, '!', 0x00}; // aCC1! -> aCC9!
uint8_t CONTINUOUS_MEASUREMENT_CRC[] = {0x00, 'R', 'C', 0x00, '!', 0x00}; // aRC0! -> aRC9!

typedef struct {
		UART_HandleTypeDef *huart;
} SDI12_TypeDef;

void SDI12_Init(UART_HandleTypeDef *huart);
int SDI12_GetDeviceId(uint8_t *addr);

#endif // SDI12_
