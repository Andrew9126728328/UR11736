/*
 * eeprom.h
 *
 *  Created on: Nov 1, 2016
 *      Author: andrew
 */

#ifndef EEPROM_H_
#define EEPROM_H_

#include "stm32f2xx_hal.h"
#include "gpio.h"
#include "spi.h"
#include "crc.h"
#include "iwdg.h"
#include "ur11435_lib.h"

#define EEPROM_MAC_ADR	(0xFA)
#define EEPROM_MAC_NMB	(6)
/*
 * Function pointers for structure
*/
typedef void (*type_eeprom_initialize)(SPI_TypeDef *SPI_Control, GPIO_TypeDef * GPIOx, uint16_t GPIO_Pin );
typedef void (*type_eeprom_write)( uint8_t eeprom_address, uint16_t number, uint8_t *from_buffer);
typedef void (*type_eeprom_read)( uint8_t eeprom_address, uint16_t number, uint8_t *to_buffer );
typedef void (*type_eeprom_write_enable)(void);
typedef void (*type_eeprom_write_disable)(void);
typedef void (*type_eeprom_read_status)(uint8_t *status);
typedef void (*type_eeprom_write_status)(uint8_t *status);
typedef uint32_t (*type_eeprom_crc)(config_t *ee_config);

typedef struct {								/* EEPROM object */
	SPI_HandleTypeDef 			*SPI_Handle;
	GPIO_TypeDef 				*GPIOx_CS;
	uint16_t 					ee_Pin_CS;
	type_eeprom_initialize 		ee_initialize;
	type_eeprom_write 			ee_write;
	type_eeprom_read 			ee_read;
	type_eeprom_write_enable	ee_write_enable;
	type_eeprom_write_disable	ee_write_disable;
	type_eeprom_read_status		ee_read_status;
	type_eeprom_write_status	ee_write_status;
	type_eeprom_crc				ee_crc_calculate;
} eeprom_t ;

extern eeprom_t eeprom;

#endif /* EEPROM_H_ */
