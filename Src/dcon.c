#include "stm32f2xx_hal.h"
#include "cmsis_os.h"
#include "usart.h"
#include "tim.h"
#include "modbus_slave.h"
#include "dcon.h"
#include <string.h>
#include <stdlib.h>


enum {I7041=0,I7051,I7043,I7063,I7065,I7055,NL_16DI,NL_16DO,NL_8AI}; /* DCON moduls on indexes */

/* description of the types of moduls on indexes */
struct
{
    uint8_t	    in;     /* Number of inputs in the module */
	uint8_t	    out;    /* Number of outputs in the module */
	char    *read_cmd;
}const dcon_lib_dio[] =
{   /*  in  out     */
	/* Digital inputs, outputs */
    {	14,	0,  "\r"	},      /* For I7041 */
    {	16,	0,  "@AA\r"	},      /* For I7051 */
    {	0,	16, "@AA\r"	},      /* For I7043 */
    {	8,	3,  "\r"	},      /* For I7063 */
    {	4,	5,  "\r"	},      /* For I7065 */
    {	8,	8,  "\r"	},      /* For I7065 */
    {	16,	0,  "@AA\r"	},      /* For NL_16DI */
    {	0,	16, "$AA6\r"},      /* For NL_16DO */
    /* Analog inputs outputs */
    {	8,	0, 	"#AA\r"	},      /* For NL_8AI */
};
/* description of the modules in the system */
struct dcon_t
{
    uint8_t            address;         /* DCON address of the module */
    uint8_t            type;            /* The module type, see dcon_lib[] */
    uint16_t           modbus_reg_in;   /* Offset in the ModBus registers for inputs (if present) */
    uint16_t           modbus_reg_out;  /* Offset in the ModBus registers for outputs (if present) */
}const dcon_modules[] =
{ /*   adr    type      modbus reg   in    out  */
//  {   2,    NL_16DI,    MODULE_01_DI,    0  },        /* The modules, which only have inputs, set 0 into Modbus out reg */
//   {   3,    NL_16DO,    DCON_03_DI,  DCON_03_DO  },
//    {   4,    NL_16DO,    DCON_04_DI,  DCON_04_DO  },
//    {   5,    NL_16DI,    DCON_05_DI,    0  },          /* Each module must have a image into ModBus in reg */
//    {   6,    NL_16DI,    DCON_06_DI,    0  },          /* The modules, which only have inputs, set 0 into Modbus out reg */
//    {   7,    NL_16DI,    DCON_07_DI,    0 },   /* для модуля дискретных выходов в переменной DCON_07_DI хранится состояние выходов, считанное из модуля */
//    {   8,    NL_16DO,    DCON_08_DI,  DCON_08_DO  },   /* а в переменной DCON_07_DO хранится состояние выходов, которое нужно передать в модуль */
//    {   9,    NL_8AI,     DCON_09_AI,    0  },
//    {   10,   NL_16DI,    DCON_10_DI,    0  },
//    {   11,   NL_16DO,    DCON_11_DI,  DCON_11_DO  },
//    {   12,   NL_16DI,    DCON_12_DI,    0  },
//    {   13,   NL_16DI,    DCON_13_DI,    0  },
//    {   14,   NL_16DO,    DCON_14_DI,  DCON_14_DO  },
//    {   15,   NL_16DI,    DCON_15_DI,    0  },
//    {   16,   NL_16DO,    DCON_16_DI,  DCON_16_DO  },
//    {   17,   NL_16DI,    DCON_17_DI,    0  },
//    {   18,   NL_16DI,    DCON_18_DI,    0  },
//   {   19,   NL_16DO,    DCON_19_DI,  DCON_19_DO  },
};
uint16_t DCON_current_read_ndx,DCON_current_write_ndx;
uint8_t *DCON_request;
volatile uint16_t    DCON_net_time;
extern xSemaphoreHandle myMutexModbusSlaveHandle;
/* Private functions **********************************************************/
static void dcon_send(uint8_t *message);
static void dcon_add_CRC(uint8_t *dcon_array);
static uint8_t dcon_check_CRC(uint8_t *dcon_array);
static uint8_t hex2asc(uint8_t hex);
static uint8_t asc2hex(uint8_t asc, uint8_t *hex);
/************************************************************************/
uint16_t dcon_modules_number(void)
{
    return sizeof(dcon_modules)/sizeof(struct dcon_t);
}
void dcon_init(void)
{
    uint16_t i;
    for(i=0;i<sizeof(dcon_modules)/sizeof(struct dcon_t);++i)   /* Clear ModBus registers for inputs */
    {
        if(dcon_modules[i].modbus_reg_in)
            ModBus_regs[dcon_modules[i].modbus_reg_in] = 0;
    }
    for(i=0;i<sizeof(dcon_modules)/sizeof(struct dcon_t);++i)   /* Find first DCON module of inputs */
    {
        if(dcon_modules[i].modbus_reg_in)
            break;
    }
    DCON_current_read_ndx = i;
    DCON_request = (uint8_t*)pvPortMalloc(DCON_REQUEST_LENGTH); /* Allocate space for buffer */
}
void dcon_next_inputs_module(void)                              /* Find next DCON module of inputs */
{
    do
    {
        if(++DCON_current_read_ndx >= sizeof(dcon_modules)/sizeof(struct dcon_t))
        {
//            ModBus_regs[DCON_NET_CIRCLE] = DCON_net_time;
            DCON_current_read_ndx = 0;
            DCON_net_time = 0;
        }
    }while(!dcon_modules[DCON_current_read_ndx].modbus_reg_in);
}
void dcon_dio_read_inputs(void)                                 /* To read current module of inputs */
{
    if(dcon_modules_number())
    {
    	uint8_t *cmd_to = &DCON_request[0];
    	uint8_t *cmd_from = (uint8_t*)dcon_lib_dio[dcon_modules[DCON_current_read_ndx].type].read_cmd;
    	do {
    		*cmd_to++ = *cmd_from++;
    	}while(*cmd_from);
    	/* Make command */
//		DCON_request[0] = '@';
//		DCON_request[0] = '$';
    	DCON_request[1] = hex2asc(dcon_modules[DCON_current_read_ndx].address >> 4);
    	DCON_request[2] = hex2asc(dcon_modules[DCON_current_read_ndx].address);
//		DCON_request[3] = '6';
//		DCON_request[4] = '\r';
//		DCON_request[3] = '\r';
#ifdef DCON_CHECK_CRC
    	dcon_add_CRC(DCON_request);                                 /* If CRC is enabled - add CRC into the request*/
#endif
    	/* To Send The request command */
    	dcon_send(DCON_request);
    }
}
uint8_t dcon_write_check(uint16_t i)
{
    uint8_t ret = pdFALSE;
    if(dcon_modules_number())
    {
    	/* The Register for get data from module */
    	uint16_t *ptr_ModBus_regs_out = &ModBus_regs[dcon_modules[i].modbus_reg_out];
    	/* The Register for put data from module */
    	uint16_t *ptr_ModBus_regs_in = &ModBus_regs[dcon_modules[i].modbus_reg_in];

    	xSemaphoreTake( myMutexModbusSlaveHandle, portMAX_DELAY );             /* Guard ModBus Slave process */
    	switch(dcon_lib_dio[dcon_modules[i].type].out)
    	{
    		case 0:
    			break;
    		case 1:
    		case 2:
    		case 3:
    		case 4:
    		case 5:
    		case 6:
    		case 7:
    		case 8:
    			if(*ptr_ModBus_regs_out != ((*ptr_ModBus_regs_in) >> 8))
    			{
    				*ptr_ModBus_regs_in = (*ptr_ModBus_regs_in & 0xff) | ((*ptr_ModBus_regs_out) << 8);
    				ret = pdTRUE;
    			}
    			break;
    		default:
    			if(*ptr_ModBus_regs_out != *ptr_ModBus_regs_in)
    			{
    				*ptr_ModBus_regs_in = *ptr_ModBus_regs_out;
    				ret = pdTRUE;
    			}
    			break;
    	}
    	xSemaphoreGive(myMutexModbusSlaveHandle);                              /* Remove the guard ModBus Slave process */
    	DCON_current_write_ndx = i;
    }
    return ret;
}
void dcon_dio_write_outputs(void)
{
	if(dcon_modules_number())
	{
		uint16_t i = 0;
		/* The Register for get data from module */
		uint16_t *ptr_ModBus_regs = &ModBus_regs[dcon_modules[DCON_current_write_ndx].modbus_reg_out];
		DCON_request[i++] = '@';
		DCON_request[i++] = hex2asc(dcon_modules[DCON_current_write_ndx].address >> 4);
		DCON_request[i++] = hex2asc(dcon_modules[DCON_current_write_ndx].address);
		xSemaphoreTake( myMutexModbusSlaveHandle, portMAX_DELAY );             /* Guard ModBus Slave process */
		switch(dcon_lib_dio[dcon_modules[DCON_current_write_ndx].type].out)
		{
			case 1:
			case 2:
			case 3:
			case 4:
				DCON_request[i++] = hex2asc((char)*ptr_ModBus_regs);
				break;
			case 5:
			case 6:
			case 7:
			case 8:
				DCON_request[i++] = hex2asc((char)(*ptr_ModBus_regs>>4));
				DCON_request[i++] = hex2asc((char)*ptr_ModBus_regs);
				break;
			default:
				DCON_request[i++] = hex2asc((char)(*ptr_ModBus_regs>>12));
				DCON_request[i++] = hex2asc((char)(*ptr_ModBus_regs>>8));
				DCON_request[i++] = hex2asc((char)(*ptr_ModBus_regs>>4));
				DCON_request[i++] = hex2asc((char)*ptr_ModBus_regs);
				break;
		}
		xSemaphoreGive(myMutexModbusSlaveHandle);                              /* Remove the guard ModBus Slave process */
		DCON_request[i++] = '\r';
#ifdef DCON_CHECK_CRC
    	dcon_add_CRC(DCON_request);                                 /* If CRC is enabled - add CRC into the request*/
#endif
    	/* To Send The request command */
    	dcon_send(DCON_request);
	}
}


void dcon_write_inputs_to_modbus_reg(uint8_t *buffer)
{
	if(dcon_modules_number())
	{
#ifdef DCON_CHECK_CRC
		if(dcon_check_CRC(buffer) != pdTRUE)                    /* If CRC enabled - check */
        	return;                                             /* If CRC is bad - quit */
#endif
    	/* The Register for put data from module */
    	uint16_t *ptr_ModBus_regs = &ModBus_regs[dcon_modules[DCON_current_read_ndx].modbus_reg_in];
    	uint8_t hex;
                                                            /* Put four nibbles */
    	if(dcon_modules[DCON_current_read_ndx].type < NL_8AI)	/* digital inputs */
    	{
    		asc2hex(buffer[1], &hex);
        	*ptr_ModBus_regs = (hex << 12);
        	asc2hex(buffer[2], &hex);
        	*ptr_ModBus_regs |= (hex << 8);
        	asc2hex(buffer[3], &hex);
        	*ptr_ModBus_regs |= (hex << 4);
        	asc2hex(buffer[4], &hex);
        	*ptr_ModBus_regs |= hex;
    	}else													/* analog inputs */
    	{
    		uint8_t i;
    		uint8_t *ptr_buffer = &buffer[1];
    		for(i=0;i<dcon_lib_dio[dcon_modules[DCON_current_read_ndx].type].in;++i)
    		{
        		asc2hex(*ptr_buffer++, &hex);
            	*ptr_ModBus_regs = (hex << 12);
            	asc2hex(*ptr_buffer++, &hex);
            	*ptr_ModBus_regs |= (hex << 8);
            	asc2hex(*ptr_buffer++, &hex);
            	*ptr_ModBus_regs |= (hex << 4);
            	asc2hex(*ptr_buffer++, &hex);
            	*ptr_ModBus_regs++ |= hex;
    		}
    	}

    	dcon_set_register('r');
	}
}
void dcon_set_register(uint8_t mode)
{
/*	uint32_t *ptr = (uint32_t*)&ModBus_regs[DCON_REGISTER];
    if(mode == 'r')
    {
    	if(dcon_modules[DCON_current_read_ndx].address < 16)
    	{
    		*ptr |= (1<<dcon_modules[DCON_current_read_ndx].address);
    	}
    }
    else
    {
    	if(dcon_modules[DCON_current_write_ndx].address < 16)
    	{
    		*ptr |= (1<<dcon_modules[DCON_current_write_ndx].address);
    	}
    }
*/
}
void dcon_reset_register(uint8_t mode)
{
/*	uint32_t *ptr = (uint32_t*)&ModBus_regs[DCON_REGISTER];
	if(mode == 'r')
    {
    	if(dcon_modules[DCON_current_read_ndx].address < 16)
    	{
    		*ptr &= ~(1<<dcon_modules[DCON_current_read_ndx].address);
    	}
    }
    else
    {
        if(dcon_modules[DCON_current_write_ndx].address < 16)
        {
        	*ptr &= ~(1<<dcon_modules[DCON_current_write_ndx].address);
        }
    }
*/
}
static void dcon_send(uint8_t *message)
{
    uint8_t message_size = 0;
    while(message[message_size++] != DCON_CR);
    DCON_TE_ON;                                             /* Turn On The RS-485 Transmitter */
    HAL_UART_Transmit_IT(&hDCON_PORT, message,message_size);/* Send the message via RS-485 Transmitter */
}
static void dcon_add_CRC(uint8_t *dcon_array)
{
    uint8_t crc_tmp = 0;
    while(*dcon_array != DCON_CR)
    {
        crc_tmp += (uint8_t)*dcon_array++;      /* Calculate CRC */
    }
    *dcon_array++ = hex2asc(crc_tmp >> 4);      /* Write CRC into DCON request */
    *dcon_array++ = hex2asc(crc_tmp);
    *dcon_array++ = DCON_CR;
    *dcon_array = '\0';
}
static uint8_t dcon_check_CRC(uint8_t *dcon_array)
{
    uint8_t *ptr_CRC = dcon_array;
    uint8_t crc,crc_tmp;
    while(*ptr_CRC != DCON_CR)          /* Find End of DCON frame */
        ptr_CRC++;

    asc2hex(*(--ptr_CRC), &crc);        /* Lower digit CRC */
    asc2hex(*(--ptr_CRC), &crc_tmp);    /* Upper digit CRC */
    crc |= (crc_tmp << 4);

    crc_tmp = 0;
    while(ptr_CRC != dcon_array)        /* Calculate CRC */
        crc_tmp += *(--ptr_CRC);
    if(crc == crc_tmp)
        return pdTRUE;
    else
        return pdFALSE;
}
/*static void dcon_module_name_request(uint8_t *dcon_array,uint8_t address)
{
	if(address)
	{
		dcon_array[0] = '$';
		dcon_array[1] = hex2asc(address >> 4);
		dcon_array[2] = hex2asc(address);
		dcon_array[3] = 'M';
		dcon_array[4] = DCON_CR;
		dcon_array[5] = '\0';
	}
}*/
void dcon_host_OK_request(void)
{
    str_copy(DCON_request,(uint8_t*)"~**\r");
#ifdef DCON_CHECK_CRC
    dcon_add_CRC(DCON_request);                                 /* If CRC is enabled - add CRC into the request*/
#endif
    /* To Send The request command */
	dcon_send(DCON_request);
}
uint16_t dcon_current_vodule(char req)
{
	return (req == 'w')? DCON_current_write_ndx:DCON_current_read_ndx;
}
static const uint8_t hex2asc_table[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
static uint8_t hex2asc(uint8_t hex)
{
	return hex2asc_table[hex&0x0F];
}
static uint8_t asc2hex(uint8_t asc, uint8_t *hex)
{
	uint8_t result = pdTRUE;

	if((asc>='0') && (asc<='9'))
		*hex = asc-'0';
	else if((asc>='A') && (asc<='F'))
		*hex = asc-'7';
	else if((asc>='a') && (asc<='f'))
		*hex = asc-'W';
    else
		result = pdFALSE;
	return result;
}
void str_copy(uint8_t *to,uint8_t *from)
{
	while(*from)
		*to++ = *from++;
	*to = '\0';
}
void str_cat(uint8_t *to,uint8_t *from)
{
	while(*to)
		to++;
	str_copy(to,from);
}
