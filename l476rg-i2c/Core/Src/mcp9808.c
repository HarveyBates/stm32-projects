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
 * Set upper and lower temperature alarm limits.
 *
 * @todo Check this for errors.
 *
 * @param limit The temperature limit on which to enforce.
 * @returns res HAL status code.
 */
HAL_StatusTypeDef MCP9808_SetTemperatureLimits(int8_t limit) {
	MCP9808_REG_TypeDef reg = MCP9808_T_UPPER_REG;

	uint8_t value[2];
	if (limit < 0) {
		value[0] = (limit << 8) | 0x100;
	} else {
		value[0] = limit << 8;
	}

	value[1] = limit;

	HAL_StatusTypeDef res = MCP9808_Write(&reg, value);

	return res;
}
