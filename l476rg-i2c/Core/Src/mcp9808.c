#include "mcp9808.h"

static MCP9808_TypeDef mcp9808;

/*
 * Initialise MCP9808 struct with i2c handler and address
 * of MCP9808.
 * Address (7-bits) is shifted left to make room for the read
 * write bit.
 */
void MCP9808_Init(I2C_HandleTypeDef *hi2c, uint8_t addr) {
	mcp9808.hi2c = hi2c;
	mcp9808.address = addr << 1;
}

/*
 * Preforms complete measure temperature command.
 * Resulting value will be calculated at the devices previously set
 * resolution.
 */
HAL_StatusTypeDef MCP9808_MeasureTemperature(float *temperature) {

	uint8_t reg[1] = {MCP9808_T_AMBIENT_REG};
	HAL_StatusTypeDef res = HAL_I2C_Master_Transmit(mcp9808.hi2c, mcp9808.address,
			reg, 1, HAL_MAX_DELAY);

	if(res != HAL_OK){
		return res;
	}

	uint8_t buf[2];
	res = HAL_I2C_Master_Receive(mcp9808.hi2c, mcp9808.address, buf, 2, HAL_MAX_DELAY);

	if(res != HAL_OK){
		return res;
	}

	uint8_t upper = buf[0];
	upper &= 0x1F;
	uint8_t lower = buf[1];

	if((upper & 0x10) == 0x10) {
		upper &= 0x0F;
		*temperature = 256 - (upper * 16.0) + (lower / 16.0);
	} else {
		*temperature = (upper * 16.0) + (lower / 16.0);
	}

	return HAL_OK;
}
