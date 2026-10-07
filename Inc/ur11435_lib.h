/*
 * ur11435_lib.h
 *
 *  Created on: Aug 26, 2016
 *      Author: andrew
 */

#ifndef UR11435_LIB_H_
#define UR11435_LIB_H_

//#define DEBUG_MAIN
//#define CONSOLE
//#define LOG_ENABLE
/* Bits of PLC HW version Register */
enum {HW_COM1=0,HW_COM2,HW_COM3,HW_CAN,HW_USB,HW_ETHNET,HW_EEPROM,HW_SDCARD,HW_AUX,HW_RTCLK,HW_DI,HW_DO,HW_AI,HW_AO};

#define PLC_HW_VERSION          ((1<<HW_COM2)|(1<<HW_USB)|(1<<HW_ETHNET)|(1<<HW_EEPROM)|(1<<HW_RTCLK)|(1<<HW_DI)|(1<<HW_AI))


#define GPIOx_KEY			GPIOE
#define GPIO_Pin_KEY		GPIO_PIN_0

#define DIO_TIMER_QUANTUM	10
#define DI_TIMER_BOUNCE		200
#define ADC_TIMER_QUANTUM	25
#define ADC_CHANELS_NUMBER	8
#define ADC_SAMPLE_NUMBER	32
#define ADC_OFFSET			2
#define MA_SAMPLE_NUMBER	4
#define V_REF				2500
#define V_REF_INT			1200
#define JOISTICK_1_KU		100
#define JOISTICK_2_KU		100
#define JOISTICK_3_KU		100
#define JOISTICK_4_KU		100
#define JOISTICK_1_ZERO		0
#define JOISTICK_2_ZERO		0
#define JOISTICK_3_ZERO		0
#define JOISTICK_4_ZERO		0
#define JOISTICK_ZERO_ZONE	15
#define ALT_BUTTON_ON_VAL	110
#define ALT_BUTTON_OFF_VAL	90
#define HoistTaskForward		50		//задание "вперёд" ПП при запасовке канатов
#define HoistTaskBackward		(-50)		//задание "назад" ПП при запасовке канатов
#define PullTaskForward	 		50		//задание "вперёд" ПН при запасовке канатов
#define PullTaskBackward		(-50)		//задание "назад" ПН при запасовке канатов

/******************************датчик давления**********************************/
// Резистор 200 Ом
// Давление в системе, bar    Сигнал датчика, мА    Напряжение на входе модуля, В    Значение, Dec
//          0							4						0,8						2621
//			4							6,4						1,28					4194
//			8							12,8					2,56					8388
//			10							20						4						13107

#define PneumoPressureCompOn	4194	//уставка давления, на включение компрессора
#define PneumoPressureCompOff	8388	//уставка давления, на выключение компрессора

#define EMA_FILTER
#define EMA_ORDER		16

#define RTC_TIMER_QUANTUM	100

#define QUEUE_SDCard_LENGTH		128

#define ADDRESS_CONFIG_IN_EEPROM	0

#define FLASH_EEPROM_PAGE_SIZE		0x00004000
/*Static IP ADDRESS: IP_ADDR0.IP_ADDR1.IP_ADDR2.IP_ADDR3 */
#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   255
#define IP_ADDR3   252

/*NETMASK*/
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0

/*Gateway Address*/
#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   255
#define GW_ADDR3   254

/*Ethernet Link Test Time */
#define TIME_WAITING_ETH_LINK	500

#define BVR_MODBUS_REGS_PER		20
#define BVR_NUMBER				4
#define UIT_MODBUS_REGS_PER		10
#define UIT_NUMBER				32
#define UIN_MODBUS_REGS_PER		10
#define UIN_NUMBER				32
#define IKP_MODBUS_REGS_PER		10
#define IKP_NUMBER				16
#define M7022_WR				2

#include "cmsis_os.h"
#include "stm32f2xx_hal.h"

typedef enum {RED=0,GREEN} color_t;
typedef enum {DI_OFF=0,DI_ON} di_state_t;
enum {JOIST_BUTTON_ALT = 0};													/* Configuration reg bits */
enum {HOIST_ZERO = 0,SWING_ZERO,CROWD_ZERO,EXTRA_ZERO,HORN_BUTTON,ODK_BUTTON};	/* DI register bits */
typedef union
{
	struct cfg_t
	{
		uint32_t 	reg;					/* Configuration register */
		uint32_t 	COM2_high_baudrate;		/* DCON baudrate */
		uint32_t 	COM2_low_baudrate;		/* Modbus master over DCON baudrate */
		int16_t 	Joistick_ku[4];			/* Gains for Joisticks */
		int16_t 	Joistick_zero[4];		/* Zero offset for Joisticks */
		uint8_t		ip_address[4];			/* Local IP address */
		uint8_t		netmask[4];				/* Netmask */
		uint8_t		gateway[4];				/* Default gateway */
		uint32_t	crc;
	}cfg;
	uint8_t 		data[sizeof(struct cfg_t)];
}config_t;

typedef struct{
    uint8_t *buffer;
    uint16_t counter;
}buffer8_t;

typedef struct
{
	GPIO_TypeDef 	*port;
	uint32_t		pin;
}dio_t;
typedef struct
{
	uint16_t timer;
	uint16_t time;
	di_state_t state;
}di_prepare_t;
/***********************/
/* Data Logging System */
typedef enum {	LSTART = 0,LERROR,LNETERROR,
				LWARNING =100,
				LSYSEVENT = 200,LNETEVENT,LDEVENT,LAEVENT,
				LDTREND = 300,LATREND}log_level_t;
typedef struct
{
	uint16_t	hours_minutes;
	uint16_t	seconds;
	uint16_t	level;
	uint32_t	value;
	uint8_t		*message;
}log_record_t;
/***********************/
extern config_t *config_running;
extern const config_t config_default;
extern const dio_t dinputs[];
extern const dio_t doutputs[];
extern xTimerHandle myTimerLEDHandle;

extern buffer8_t com3_RxTx;
extern uint8_t com3_RxByte;
enum{DCON_MODE=0,MODBUS_MODE};
extern uint8_t com3_Mode;

void led_on(color_t);
void led_off(void);
void led_spark(color_t, uint16_t);
void led_blink(color_t, uint16_t);
void CallbackLED(xTimerHandle pxTimer);

/* USB BreakTime ModBus timer */
void usb_stop_break(void);
void usb_start_break(void);
void MX_TIM7_Init(uint32_t baudrate,uint32_t period);
/* COM1 BreakTime ModBus timer */
void com1_stop_break(void);
void com1_start_break(void);
/* ModBuas over DCON BreakTime ModBus timer */
void com3_stop_break(void) ;
void com3_start_break(void);
#endif /* UR11435_LIB_H_ */
