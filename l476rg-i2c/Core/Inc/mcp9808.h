#ifndef MCP9808_H_
#define MCP9808_H_

#include <string.h>
#include <stdio.h>
#include "main.h"

typedef struct {
		I2C_HandleTypeDef *hi2c;
		uint8_t address;
} MCP9808_TypeDef;

typedef enum {
	MCP9808_CONFIG_REG = 0x01,
	MCP9808_T_UPPER_REG = 0x02,
	MCP9808_T_LOWER_REG =0x03,
	MCP9808_T_CRIT_REG = 0x04,
	MCP9808_T_AMBIENT_REG = 0x05,
	MCP9808_MANUFACTURER_REG = 0x06,
	MCP9808_DEVICE_ID_REG = 0x07,
	MCP9808_RESOLUTION_REG = 0x08
} MCP9808_REG_TypeDef;

void MCP9808_Init(I2C_HandleTypeDef *hi2c, uint8_t addr);
HAL_StatusTypeDef MCP9808_MeasureTemperature(float *temperature);


#endif // MCP9808_H_
