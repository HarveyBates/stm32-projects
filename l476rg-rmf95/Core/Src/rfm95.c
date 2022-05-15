#include "rfm95.h"

RFM95_TypeDef rfm95;

void RFM95_Init(SPI_HandleTypeDef *hspi) {
	rfm95.hspi = *hspi;
	GPIO_TypeDef *CS_Port;
	GPIO_TypeDef *CS_Pin;
}
