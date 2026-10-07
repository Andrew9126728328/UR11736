/*
 * eeprom.c
 *
 *  Created on: Nov 1, 2016
 *      Author: andrew
 */

#include "eeprom.h"

/* eeprom commands */
const uint8_t READ	=0x03;
const uint8_t WRITE	=0x02;
const uint8_t WRDI	=0x04;
const uint8_t WREN	=0x06;
const uint8_t RDSR	=0x05;
const uint8_t WRSR	=0x01;
/* eeprom status register */
#define WIP		(1<<0)		/* Write in process */
#define WEL		(1<<1)		/* Write enable */
#define BP0		(1<<2)		/* Block protection 0 */
#define BP1		(1<<3)		/* Block protection 1 */

/* EUI-48 for Eth MAC address */
#define EUI_48_address	0xFA
#define EUI_48_bytes	0x06
/* Write-protected arrea */
#define END_WR_EEPROM	0xC0
#define END_RD_EEPROM	0xFF

void MX_SPI_Init(SPI_TypeDef *hspi);
void eeprom_initialize(SPI_TypeDef *SPI_Control, GPIO_TypeDef * GPIOx, uint16_t GPIO_Pin );
void eeprom_write( uint8_t eeprom_address, uint16_t number, uint8_t *from_buffer);
void eeprom_read( uint8_t eeprom_address, uint16_t number, uint8_t *to_buffer );
void eeprom_write_enable(void);
void eeprom_write_disable(void);
void eeprom_read_status(uint8_t *status);
void eeprom_write_status(uint8_t *status);
uint32_t eeprom_crc(config_t *ee_config);
/* EEPROM object */
eeprom_t eeprom = { 0,0,0,
					eeprom_initialize,
					eeprom_write,
					eeprom_read,
					eeprom_write_enable,
					eeprom_write_disable,
					eeprom_read_status,
					eeprom_write_status,
					eeprom_crc};

SPI_HandleTypeDef eeprom_hspi;
/* SPI init */
void MX_SPI_Init(SPI_TypeDef *spi)
{

  eeprom_hspi.Instance = spi;
  eeprom_hspi.Init.Mode = SPI_MODE_MASTER;
  eeprom_hspi.Init.Direction = SPI_DIRECTION_2LINES;
  eeprom_hspi.Init.DataSize = SPI_DATASIZE_8BIT;
  eeprom_hspi.Init.CLKPolarity = SPI_POLARITY_LOW;
  eeprom_hspi.Init.CLKPhase = SPI_PHASE_1EDGE;
  eeprom_hspi.Init.NSS = SPI_NSS_SOFT;
  eeprom_hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  eeprom_hspi.Init.FirstBit = SPI_FIRSTBIT_MSB;
  eeprom_hspi.Init.TIMode = SPI_TIMODE_DISABLE;
  eeprom_hspi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  eeprom_hspi.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&eeprom_hspi) != HAL_OK)
  {
    Error_Handler();
  }

}
/* Calculate CRC */
uint32_t eeprom_crc(config_t *ee_config)
{
	return HAL_CRC_Calculate(&hcrc, (uint32_t*)ee_config->data, sizeof(ee_config->data)/4-1);
}

void eeprom_initialize(SPI_TypeDef *SPI_Control, GPIO_TypeDef * GPIOx, uint16_t GPIO_Pin )
{
	eeprom.SPI_Handle 	= &eeprom_hspi;		/* Set SPI on eeprom */
	eeprom.GPIOx_CS 	= GPIOx;			/* Set port CS on eeprom */
	eeprom.ee_Pin_CS 		= GPIO_Pin;			/* Set pin CS on eeprom */
	MX_SPI_Init(SPI_Control);				/* SPI init */
	HAL_GPIO_WritePin(eeprom.GPIOx_CS,eeprom.ee_Pin_CS,SET); /* CS up */


}
void eeprom_one_page_write( uint8_t eeprom_address, uint16_t number, uint8_t *from_buffer)
{
	uint8_t status;
	status=WIP;
	while(status & WIP)					/* Wait eeprom write readiness */
	{
		eeprom.ee_read_status(&status);
		HAL_IWDG_Refresh(&hiwdg);
	}
	eeprom.ee_write_enable();											/* Write enable eeprom */
	HAL_GPIO_WritePin(eeprom.GPIOx_CS,eeprom.ee_Pin_CS,RESET);		/* CS down */
	HAL_SPI_Transmit(eeprom.SPI_Handle, (uint8_t*)&WRITE, 1, 5);	/* Send command */
	HAL_SPI_Transmit(eeprom.SPI_Handle, &eeprom_address, 1, 5);	/* Send address */
	HAL_SPI_Transmit(eeprom.SPI_Handle, from_buffer, number, 25);	/* Send data */
	HAL_GPIO_WritePin(eeprom.GPIOx_CS,eeprom.ee_Pin_CS,SET);			/* CS up */
}
void eeprom_write( uint8_t eeprom_address, uint16_t number, uint8_t *from_buffer)
{
	if((eeprom_address+(number-1))>END_WR_EEPROM) return; 		/* Out of memory */
	while(((eeprom_address & 0x0F)+number) > 16)		/* More than one page */
	{
		uint16_t nmb = 16 - (eeprom_address & 0x0F);
		eeprom_one_page_write( eeprom_address, nmb, from_buffer);	/* Write one page */
		eeprom_address += nmb;
		number -= nmb;
		from_buffer += nmb;
	}
	if(number)
		eeprom_one_page_write( eeprom_address, number, from_buffer);		/* Write tail */
}
void eeprom_read( uint8_t eeprom_address, uint16_t number, uint8_t *to_buffer )
{
	if((eeprom_address+(number-1)) <= END_RD_EEPROM)
	{
		HAL_GPIO_WritePin(eeprom.GPIOx_CS,eeprom.ee_Pin_CS,RESET);		/* CS down */
		HAL_SPI_Transmit(eeprom.SPI_Handle, (uint8_t*)&READ, 1, 5);	/* Send command */
		HAL_SPI_Transmit(eeprom.SPI_Handle, &eeprom_address, 1, 5);	/* Send address */
		HAL_SPI_Receive(eeprom.SPI_Handle, to_buffer, number, 10);		/* Receive data */
		HAL_GPIO_WritePin(eeprom.GPIOx_CS,eeprom.ee_Pin_CS,SET);			/* CS up */
	}
}
void eeprom_write_enable(void)
{
	HAL_GPIO_WritePin(eeprom.GPIOx_CS,eeprom.ee_Pin_CS,RESET);			/* CS down */
	HAL_SPI_Transmit(eeprom.SPI_Handle, (uint8_t*)&WREN, 1, 5);		/* Send command */
	HAL_GPIO_WritePin(eeprom.GPIOx_CS,eeprom.ee_Pin_CS,SET);				/* CS up */
}
void eeprom_write_disable(void)
{
	HAL_GPIO_WritePin(eeprom.GPIOx_CS,eeprom.ee_Pin_CS,RESET);			/* CS down */
	HAL_SPI_Transmit(eeprom.SPI_Handle, (uint8_t*)&WRDI, 1, 5);		/* Send command */
	HAL_GPIO_WritePin(eeprom.GPIOx_CS,eeprom.ee_Pin_CS,SET);				/* CS up */
}
void eeprom_read_status(uint8_t *status)
{
	HAL_GPIO_WritePin(eeprom.GPIOx_CS,eeprom.ee_Pin_CS,RESET);			/* CS down */
	HAL_SPI_Transmit(eeprom.SPI_Handle, (uint8_t*)&RDSR, 1, 5);		/* Send command */
	HAL_SPI_Receive(eeprom.SPI_Handle, status, 1, 5);					/* Receive status register */
	HAL_GPIO_WritePin(eeprom.GPIOx_CS,eeprom.ee_Pin_CS,SET);				/* CS up */
}
void eeprom_write_status(uint8_t *status)
{

}
