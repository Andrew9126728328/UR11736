#include "stm32f2xx_hal.h"
#include "cmsis_os.h"
#include "modbus_slave.h"
#include "plc_image.h"

/*
*********************** Must be mandatory ***********************************************
 */
VAR(sig_reset,      0xFFFF,     DISCRETE,   0);          // Сигнал RESET (фиктивная переменная)
VAR(sig_empty,      0xFFFF,     DISCRETE,   0);          // Сигнал EMPTY (фиктивная переменная)
/*
*********************** User variables ***********************************************/

/******************** Discrete variable *************************/
VAR(discrete_example,     MODULE_01_DI, DISCRETE,    BIT_0);
/******************** Analog variable ***************************/
VAR(analog_example,        ADC_00, ANALOG, 	 UNSIGNED);

/******************** Software timers ***************************/
VAR(timer_example,    	TIMERS_BLOCK_1, TIMER,    BIT_0);
/*
*********************** Variable reformat table ***********************************************
*/
const reformat_t reformat_table[]=
{
//		{FROM_MBH+3,			MBH_ADC,				split},
};
const uint32_t sizeof_reformat_table = sizeof(reformat_table)/sizeof(reformat_t);
/*
*********************** Variable operating time table ***********************************************
*/
const operating_time_t operating_time_table[]=
{
//		{&di_Ct_HV,O_TIME_1,RTC_BKP_DR1},
};
const uint32_t sizeof_operating_time_table = sizeof(operating_time_table)/sizeof(operating_time_t);
