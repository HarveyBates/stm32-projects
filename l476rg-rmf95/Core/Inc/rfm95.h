#ifndef RFM95_H_
#define RFM95_H_

#include "main.h"

/**
 * Handles RFM95 instance.
 */
typedef struct {
	SPI_HandleTypeDef *hspi; ///< SPI handler
	GPIO_TypeDef *CS_Port; ///< Chip select (CS) GPIO port
	uint16_t *CS_Pin; ///< Chip select (CS) GPIO pin
	GPIO_TypeDef *RST_Port; ///< Reset trigger port
	uint16_t *RST_Pin; ///< Reset trigger pin
	uint8_t *DEVEUI; ///< Device EUI (8 bytes)
	uint8_t *APPEUI; ///< Application EUI (8 bytes)
	uint8_t *APPKEY; ///< Application key (16 bytes)
} RFM95_TypeDef;

/**
 * RFM95 relevant registers for LoRa.
 */
typedef enum {
	RFM95_FIFO_RegAccess = 0x00, ///< First in last out read write access
	RFM95_OP_Mode = 0x01, ///< Operation mode (LoRa should be configured)
	RFM95_MSB_CarrierFreq = 0x06, ///< MSB of carrier freqency
	RFM95_IB_CarrierFreq = 0x07, ///< Intermediate bits of carrier freqency
	RFM95_LSB_CarrierFreq = 0x08, ///< LSB of carrier freqency
	RFM95_PA_Selection = 0x09, ///< Power amplifier selection
	RFM95_PA_RampTime = 0x0A, ///< Power amplifier ramp time
	RFM95_OverCurrent = 0x0B, ///< Over current protection control
	RFM95_LNA_Settings = 0x0C, ///< TBD
	RFM95_FIFO_SpiPtr = 0x0D, ///< SPI pointer
	RFM95_FIFO_RSSI_StartTx = 0x0E, ///< Start TX data
	RFM95_FIFO_RSSI_StartRx = 0x0F, ///< Start RX data
	RFM95_IRQ_Flags = 0x10, ///< LoRa state flages RSSI threshold
	RFM95_IRQ_FlagsMask = 0x11, ///< Operation flag mask (RSSI value in dBm)
	RFM95_MSB_IfFreq = 0x12, ///< MSB IF frequency
	RFM95_LSB_IfFreq = 0x13, ///< LSB IF frequceny
	RFM95_MSB_RecTimeout = 0x14, ///< OOK demodulator
	RFM95_LSB_RecTimeout = 0x15, ///
	RFM95_TX_Config = 0x16,
	RFM95_PayloadLenght = 0x17,
	RFM95_MSB_PreambleLength = 0x18,
	RFM95_LSB_PreambleLenght = 0x19,
	RFM95_ModulationCfg = 0x1A,
	RFM95_RfMode = 0x1B,
	RFM95_HopPeriod = 0x1C,
	RFM95_RX_BytesLenght = 0x1D,
	RFM95_RX_HeaderInfo = 0x1E,
	RFM95_RX_HeaderCount = 0x1F,
	RFM95_RX_PacketCount = 0x20,
	RFM95_ModemStatus = 0x21,
	RFM95_SNR_Packet = 0x22,
	RFM95_RSSI_Current = 0x23,
	RFM95_RSSI_LastPacket = 0x24,
	RFM95_HopChannel = 0x25,
	RFM95_RX_DataAddres = 0x26
} RFM95_Registers_TypeDef;


#endif // RFM95_H_
