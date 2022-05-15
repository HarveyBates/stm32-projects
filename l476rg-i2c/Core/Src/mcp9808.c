/*
 ******************************************************************************
 * @file           : mcp9808.c
 * @brief          : Read and write to MCP9808 temperature sensor.
 * 						Built using a STM32L476RG.
 ******************************************************************************
 * 	Supports:
 * 	- Read temperature
 * 	- Set alarms
 * 	- Read alarms
 * 	- Set resolution
 * 	- Read resolution
 ******************************************************************************
 */

#include "mcp9808.h"

static MCP9808_TypeDef mcp9808;
static HAL_StatusTypeDef MCP9808_Write(MCP9808_REG_TypeDef *_reg, uint8_t *value);
static HAL_StatusTypeDef MCP9808_Read(MCP9808_REG_TypeDef *_reg, uint8_t *buf, uint8_t buf_size);

/**
 * Initialise MCP9808 struct with i2c handler and address
 * of MCP9808.
 *
 * Address (7-bits) is shifted left to make room for the read
 * write bit.
 *
 * @param hi2c A pointer to the I2C handler.
 * @param addr Address of MCP9808 on I2C bus (default 0x18).
 */
void MCP9808_Init(I2C_HandleTypeDef *hi2c, uint8_t addr) {
	mcp9808.hi2c = hi2c;
	mcp9808.address = addr << 1;
	mcp9808.resolution = MCP9808_VeryHigh_Res;
}

/**
 * Writes to a MCP9808 register with a value.
 *
 * @param _reg Pointer to a register.
 * @param value The data on which to send.
 * @returns res HAL status code.
 */
static HAL_StatusTypeDef MCP9808_Write(MCP9808_REG_TypeDef *_reg, uint8_t *value) {
	uint8_t reg[] = {*_reg, *value};
	HAL_StatusTypeDef res = HAL_I2C_Master_Transmit(mcp9808.hi2c, mcp9808.address,
			reg, sizeof(reg), HAL_MAX_DELAY);

	return res;
}

/**
 * Read data from MCP9808. Requires a write command to get a reply.
 *
 * @param _reg Pointer to a register.
 * @param buf A pointer to a buffer to store the response in.
 * @param buf_size The size of the buffer (n values).
 * @returns res HAL status code.
 */
static HAL_StatusTypeDef MCP9808_Read(MCP9808_REG_TypeDef *_reg, uint8_t *buf, uint8_t buf_size) {

	uint8_t reg[1] = {*_reg};
	HAL_StatusTypeDef res = HAL_I2C_Master_Transmit(mcp9808.hi2c, mcp9808.address,
			reg, sizeof(reg), HAL_MAX_DELAY);


	if(res != HAL_OK){
		return res;
	}

	res = HAL_I2C_Master_Receive(mcp9808.hi2c, mcp9808.address, buf, buf_size, HAL_MAX_DELAY);

	if(res != HAL_OK){
		return res;
	}

	return res;
}

/**
 * Preforms complete measure temperature command in degrees Celsius.
 * Resulting value will be calculated at the devices previously set
 * resolution.
 *
 * @param temperature A pointer to a temperature float to store a returned
 * value from.
 * @returns res HAL status code.
 */
HAL_StatusTypeDef MCP9808_MeasureTemperature(float *temperature) {

	MCP9808_REG_TypeDef reg = MCP9808_T_AMBIENT_REG;
	uint8_t buf[2];
	HAL_StatusTypeDef res = MCP9808_Read(&reg, buf, sizeof(buf));

	if(res == HAL_OK) {
		uint8_t upper = buf[0];
		upper &= 0x1F;
		uint8_t lower = buf[1];

		if((upper & 0x10) == 0x10) {
			upper &= 0x0F;
			*temperature = 256 - (upper * 16.0) + (lower / 16.0);
		} else {
			*temperature = (upper * 16.0) + (lower / 16.0);
		}
	}

	return res;
}

/**
 * Set the resolution of the temperature reading.
 *
 * Low = +0.5 (fastest 30 ms)
 * Medium = +0.25 (65 ms)
 * High = 0.125  (130 ms)
 * VeryHigh = 0.0625 (slowest 250 ms)
 *
 * @param resolution Desired resolution to switch to.
 * @returns res HAL status code.
 */
HAL_StatusTypeDef MCP9808_SetResolution(MCP9808_Resolution_TypeDef resolution) {

	MCP9808_REG_TypeDef reg = MCP9808_RESOLUTION_REG;
	uint8_t value = resolution & 0x03;

	HAL_StatusTypeDef res = MCP9808_Write(&reg, &value);

	if(res == HAL_OK) {
		mcp9808.resolution = resolution;
	}

	return res;
}

/**
 * Gets the resolution of temperature readings.
 *
 * Low = +0.5 (fastest 30 ms) 0x00
 * Medium = +0.25 (65 ms) 0x01
 * High = 0.125  (130 ms) 0x02
 * VeryHigh = 0.0625 (slowest 250 ms) 0x03
 *
 * @returns res HAL status code.
 */
HAL_StatusTypeDef MCP9808_GetResolution() {

	MCP9808_REG_TypeDef reg = MCP9808_RESOLUTION_REG;
	uint8_t buf[1];
	return MCP9808_Read(&reg, buf, sizeof(buf));
}

/**
 * Set upper, lower and critical temperature alarm limits.
 *
 * @param reg Alarm register to assign limit to.
 * @param limit The temperature limit on which to enforce.
 * @returns res HAL status code.
 */
HAL_StatusTypeDef MCP9808_SetTemperatureLimits(MCP9808_Alarm_TypeDef reg, int16_t limit) {

	uint8_t buf[2];

	if(limit < 0){
		limit = -limit;
		buf[1] = ((limit & 0xFF) >> 4) | 0x10; ///< Mask sign bit to 1 (represents negative limit)
	} else {
		buf[1] = (limit & 0xFF) >> 4;
	}
	buf[0] = (limit & 0xFF) << 4;

	uint8_t toSend[3] = {reg, buf[1], buf[0]};
	HAL_StatusTypeDef res = HAL_I2C_Master_Transmit(mcp9808.hi2c, mcp9808.address,
				toSend, sizeof(toSend), HAL_MAX_DELAY);

	return res;
}

/**
 * Reads a value from an alarm register.
 *
 * @param _reg Alarm register to read limit.
 * @param limit A pointer to store the limit in.
 * @returns res HAL status code.
 */
HAL_StatusTypeDef MCP9808_GetTemperatureLimit(MCP9808_Alarm_TypeDef _reg, int16_t *limit) {

	uint8_t reg[1] = {_reg};
	HAL_StatusTypeDef res = HAL_I2C_Master_Transmit(mcp9808.hi2c, mcp9808.address,
			reg, sizeof(reg), HAL_MAX_DELAY);


	if(res != HAL_OK){
		return res;
	}

	uint8_t buf[2];
	res = HAL_I2C_Master_Receive(mcp9808.hi2c, mcp9808.address, buf, sizeof(buf), HAL_MAX_DELAY);

	if(res != HAL_OK) {
		return res;
	}

	if(buf[0] > 0x10){
		// Negative value
		// Set sign bit to 1
		*limit = -((buf[0] & 0xEF) << 4 | (buf[1] & 0xFF) >> 4);
	} else {
		// Positive value
		// Sign bit = 0
		*limit = (buf[0] & 0xFF) << 4 | (buf[1] & 0xFF) >> 4;
	}

	return res;
}


