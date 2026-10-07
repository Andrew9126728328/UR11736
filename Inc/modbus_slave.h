#ifndef _MODBUS_SLAVE_H_
#define _MODBUS_SLAVE_H_

#include "cmsis_os.h"
#include "ur11435_lib.h"
/*---------------------------- Symbol Define -------------------------------*/
#define MODBUS_BUF_SZ 			(256)
#define COM1_BAUDRATE       	(115200UL)
#define MODBUS_BREAKTIME      	(35)

#define UART_DATA       buffer8_t
#define rxcnt           counter
#define txcnt           counter
#define txlen           counter

typedef enum {MODBUS_RTU=0,MODBUS_ASC,MODBUS_TCP} modbus_mode_t;

#define MB_FUNC_NONE							    00
#define MB_FUNC_READ_COILS						    01  /* implemented now	*/
#define MB_FUNC_READ_DISCRETE_INPUTS			    02  /* implemented now	*/
#define MB_FUNC_READ_HOLDING_REGISTER			    03	/* implemented now	*/
#define MB_FUNC_READ_INPUT_REGISTER				    04  /* implemented now	*/
#define MB_FUNC_WRITE_SINGLE_COIL				    05
#define MB_FUNC_WRITE_REGISTER					    06  /* implemented now	*/
#define MB_FUNC_WRITE_MULTIPLE_COILS			    15
#define MB_FUNC_WRITE_MULTIPLE_REGISTERS		    16	/* implemented now	*/
#define MB_FUNC_READ_CCHECK	    					0x16
#define MB_FUNC_READWRITE_MULTIPLE_REGISTERS	    23
#define MB_FUNC_ENCAPSULATED_INTERFACE_TRANSPORT    43
#define MB_FUNC_EXCEPTION						    0x80
//buffer usart
#define MODBUS_WRD_SZ 	((MODBUS_BUF_SZ-5)/2) //max quantity of words in response
#define MODBUS_DSCR_SZ 	((MODBUS_BUF_SZ-5)*8) //max quantity of discretes in response
#define MODBUS_ID		1

#define MODBUS_REGS_SYS         40
#define MODBUS_REGS_DI          10
#define MODBUS_REGS_DO          10
#define MODBUS_REGS_AI          1000//ADC_CHANELS_NUMBER*2+(BVR_NUMBER*BVR_MODBUS_REGS_PER)+(UIT_NUMBER*UIT_MODBUS_REGS_PER)+(UIN_NUMBER*UIN_MODBUS_REGS_PER)+(IKP_NUMBER*IKP_MODBUS_REGS_PER)
#define MODBUS_REGS_AO          10
#define MODBUS_REGS_VAR         512
#define MODBUS_REGS_NUM         (MODBUS_REGS_SYS+MODBUS_REGS_DI+MODBUS_REGS_DO+MODBUS_REGS_AI+MODBUS_REGS_AO+MODBUS_REGS_VAR)
#define MODBUS_SYS_BASE         0                                              //  0- 31
#define MODBUS_DI_BASE          (MODBUS_SYS_BASE	+MODBUS_REGS_SYS)
#define MODBUS_DO_BASE          (MODBUS_DI_BASE	+MODBUS_REGS_DI)
#define MODBUS_AI_BASE          (MODBUS_DO_BASE	+MODBUS_REGS_DO)
#define MODBUS_AO_BASE          (MODBUS_AI_BASE	+MODBUS_REGS_AI)
#define MODBUS_VAR_BASE         (MODBUS_AO_BASE	+MODBUS_REGS_AO)

/*************** System variable *****************/
enum {  PLC_HW_VER = MODBUS_SYS_BASE, PLC_SW_VER, PLC_SW_DATE, PLC_SW_YEAR,
        PLC_SYS_REG,PLC_LOAD,PLC_UPTIME,PLC_UPTIME_HI,
        TASK_NUMBER,HEAP_FREE,
		DAY_MONTH,YEAR,HOURS_MINUTES,SECONDS,
//        DCON_REGISTER,DCON_REGISTER_HI,DCON_NET_CIRCLE,DCON_ERRORS,DCON_TIMEOUTS,
        MODBUS_OVER_DCON_REGISTER,//MODBUS_OVER_DCON_ERRORS = MODBUS_OVER_DCON_REGISTER+4,

		};
/*************** Digital Inputs ******************/
/** DCON inputs */

enum {  ONBOARD_DI = MODBUS_DI_BASE, /*40*/
		MODULE_01_DI,
        };
/*************** Digital Outputs ******************/
/** DCON outputs*/

enum {  ONBOARD_DO = MODBUS_DO_BASE, /*50*/

      	};

/*************** Analog Intputs ******************/
enum {  ADC_00 = MODBUS_AI_BASE, //60
		ADC_01,ADC_02,ADC_03,ADC_04,ADC_05,ADC_06,ADC_07,
		FADC_00,FADC_01,FADC_02,FADC_03,FADC_04,FADC_05,FADC_06,FADC_07,
		/* 76 */
		BVR,
		/* 156 */
		UIT = BVR+(BVR_NUMBER*BVR_MODBUS_REGS_PER),
		/* 476 */
		UIN = UIT+(UIT_NUMBER*UIT_MODBUS_REGS_PER),
		/* 796 */
		IKP = UIN+(UIN_NUMBER*UIN_MODBUS_REGS_PER),
        };
/*************** Analog Outputs ******************/
enum {  AO_00 = MODBUS_AO_BASE, //1060
		AO_01,AO_02,AO_03,

		};
/*************** Internal Variables ******************/
enum {  SET_YEAR = MODBUS_VAR_BASE, //1070
		SET_MONTH,SET_DAY,SET_WEEKDAY,SET_HOUR,SET_MINUTE,SET_SECOND,SET_CALIBRATION,CALIBRATION,SET_CAL,CAL_FROM,CAL_TO,
		TIMERS_BLOCK_1,
//		TIMERS_BLOCK_2,TIMERS_BLOCK_3,
		DISCRETE_BLOCK_1,
//		DISCRETE_BLOCK_2,DISCRETE_BLOCK_3,
		CONFIG = 1100,

/**********************************************************************/
      };

/*---------------------------- Variable Define -------------------------------*/
//extern uint16_t     ModBus_regs[];
//extern uint16_t     ModBus_regs_old[];
extern uint16_t     *ModBus_regs;
extern uint16_t     *ModBus_regs_old;
extern xSemaphoreHandle    myMutexModbusSlaveHandle;
extern buffer8_t 	com1_RxTx;
extern buffer8_t 	Eth_RxTx;
extern uint8_t 		com1_RxByte;
/*----------------------------Functions Define--------------------------------*/
void MODBUS_SLAVE(UART_DATA *MODBUS,modbus_mode_t mode);
uint16_t Crc16( uint8_t * pucFrame, int usLen );
#endif /* _MODBUS_SLAVE_H_ */
