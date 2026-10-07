#include "stm32f2xx_hal.h"
#include "cmsis_os.h"
#include "modbus_master.h"
#include "usart.h"
#include "dcon.h"

struct modbus_modules_struct
{
    uint8_t     address;    /* Modbus module address */
    uint8_t     cmd;        /* Modbus Command */
    uint16_t    number;     /* Number of object */
    uint16_t    from;       /* Where to get */
    uint16_t    to;         /* Where to put */
}modbus_modules[] =
{
    {0x01,  MB_FUNC_READ_HOLDING_REGISTER,  BVR_MODBUS_REGS_PER,  0x000,  BVR+BVR_MODBUS_REGS_PER*0,},
    {0x02,  MB_FUNC_READ_HOLDING_REGISTER,  BVR_MODBUS_REGS_PER,  0x000,  BVR+BVR_MODBUS_REGS_PER*1,},
    {0x03,  MB_FUNC_READ_HOLDING_REGISTER,  BVR_MODBUS_REGS_PER,  0x000,  BVR+BVR_MODBUS_REGS_PER*2,},
    {0x04,  MB_FUNC_READ_HOLDING_REGISTER,  BVR_MODBUS_REGS_PER,  0x000,  BVR+BVR_MODBUS_REGS_PER*3,},

    {0x05,  MB_FUNC_WRITE_MULTIPLE_REGISTERS,	M7022_WR,  		  AO_00,  0x000,},
    {0x06,  MB_FUNC_WRITE_MULTIPLE_REGISTERS,	M7022_WR,  		  AO_02,  0x000,},

    {0x10,  MB_FUNC_READ_INPUT_REGISTER,  UIT_MODBUS_REGS_PER,  0x000,  UIT+UIT_MODBUS_REGS_PER*0,},
    {0x11,  MB_FUNC_READ_INPUT_REGISTER,  UIT_MODBUS_REGS_PER,  0x000,  UIT+UIT_MODBUS_REGS_PER*1,},
    {0x12,  MB_FUNC_READ_INPUT_REGISTER,  UIT_MODBUS_REGS_PER,  0x000,  UIT+UIT_MODBUS_REGS_PER*2,},
    {0x13,  MB_FUNC_READ_INPUT_REGISTER,  UIT_MODBUS_REGS_PER,  0x000,  UIT+UIT_MODBUS_REGS_PER*3,},
    {0x14,  MB_FUNC_READ_INPUT_REGISTER,  UIT_MODBUS_REGS_PER,  0x000,  UIT+UIT_MODBUS_REGS_PER*4,},
    {0x15,  MB_FUNC_READ_INPUT_REGISTER,  UIT_MODBUS_REGS_PER,  0x000,  UIT+UIT_MODBUS_REGS_PER*5,},
    {0x16,  MB_FUNC_READ_INPUT_REGISTER,  UIT_MODBUS_REGS_PER,  0x000,  UIT+UIT_MODBUS_REGS_PER*6,},
    {0x17,  MB_FUNC_READ_INPUT_REGISTER,  UIT_MODBUS_REGS_PER,  0x000,  UIT+UIT_MODBUS_REGS_PER*7,},
    {0x18,  MB_FUNC_READ_INPUT_REGISTER,  UIT_MODBUS_REGS_PER,  0x000,  UIT+UIT_MODBUS_REGS_PER*8,},
    {0x19,  MB_FUNC_READ_INPUT_REGISTER,  UIT_MODBUS_REGS_PER,  0x000,  UIT+UIT_MODBUS_REGS_PER*9,},
    {0x1A,  MB_FUNC_READ_INPUT_REGISTER,  UIT_MODBUS_REGS_PER,  0x000,  UIT+UIT_MODBUS_REGS_PER*10,},
    {0x1B,  MB_FUNC_READ_INPUT_REGISTER,  UIT_MODBUS_REGS_PER,  0x000,  UIT+UIT_MODBUS_REGS_PER*11,},
    {0x1C,  MB_FUNC_READ_INPUT_REGISTER,  UIT_MODBUS_REGS_PER,  0x000,  UIT+UIT_MODBUS_REGS_PER*12,},
    {0x1D,  MB_FUNC_READ_INPUT_REGISTER,  UIT_MODBUS_REGS_PER,  0x000,  UIT+UIT_MODBUS_REGS_PER*13,},
    {0x1E,  MB_FUNC_READ_INPUT_REGISTER,  UIT_MODBUS_REGS_PER,  0x000,  UIT+UIT_MODBUS_REGS_PER*14,},
    {0x1F,  MB_FUNC_READ_INPUT_REGISTER,  UIT_MODBUS_REGS_PER,  0x000,  UIT+UIT_MODBUS_REGS_PER*15,},
    {0x20,  MB_FUNC_READ_INPUT_REGISTER,  UIT_MODBUS_REGS_PER,  0x000,  UIT+UIT_MODBUS_REGS_PER*16,},
    {0x21,  MB_FUNC_READ_INPUT_REGISTER,  UIT_MODBUS_REGS_PER,  0x000,  UIT+UIT_MODBUS_REGS_PER*17,},
    {0x22,  MB_FUNC_READ_INPUT_REGISTER,  UIT_MODBUS_REGS_PER,  0x000,  UIT+UIT_MODBUS_REGS_PER*18,},
    {0x23,  MB_FUNC_READ_INPUT_REGISTER,  UIT_MODBUS_REGS_PER,  0x000,  UIT+UIT_MODBUS_REGS_PER*19,},
    {0x24,  MB_FUNC_READ_INPUT_REGISTER,  UIT_MODBUS_REGS_PER,  0x000,  UIT+UIT_MODBUS_REGS_PER*20,},
    {0x25,  MB_FUNC_READ_INPUT_REGISTER,  UIT_MODBUS_REGS_PER,  0x000,  UIT+UIT_MODBUS_REGS_PER*21,},
    {0x26,  MB_FUNC_READ_INPUT_REGISTER,  UIT_MODBUS_REGS_PER,  0x000,  UIT+UIT_MODBUS_REGS_PER*22,},
    {0x27,  MB_FUNC_READ_INPUT_REGISTER,  UIT_MODBUS_REGS_PER,  0x000,  UIT+UIT_MODBUS_REGS_PER*23,},
    {0x28,  MB_FUNC_READ_INPUT_REGISTER,  UIT_MODBUS_REGS_PER,  0x000,  UIT+UIT_MODBUS_REGS_PER*24,},
    {0x29,  MB_FUNC_READ_INPUT_REGISTER,  UIT_MODBUS_REGS_PER,  0x000,  UIT+UIT_MODBUS_REGS_PER*25,},
    {0x2A,  MB_FUNC_READ_INPUT_REGISTER,  UIT_MODBUS_REGS_PER,  0x000,  UIT+UIT_MODBUS_REGS_PER*26,},
    {0x2B,  MB_FUNC_READ_INPUT_REGISTER,  UIT_MODBUS_REGS_PER,  0x000,  UIT+UIT_MODBUS_REGS_PER*27,},
    {0x2C,  MB_FUNC_READ_INPUT_REGISTER,  UIT_MODBUS_REGS_PER,  0x000,  UIT+UIT_MODBUS_REGS_PER*28,},
    {0x2D,  MB_FUNC_READ_INPUT_REGISTER,  UIT_MODBUS_REGS_PER,  0x000,  UIT+UIT_MODBUS_REGS_PER*29,},
    {0x2E,  MB_FUNC_READ_INPUT_REGISTER,  UIT_MODBUS_REGS_PER,  0x000,  UIT+UIT_MODBUS_REGS_PER*30,},
    {0x2F,  MB_FUNC_READ_INPUT_REGISTER,  UIT_MODBUS_REGS_PER,  0x000,  UIT+UIT_MODBUS_REGS_PER*31,},

    {0x30,  MB_FUNC_READ_INPUT_REGISTER,  UIN_MODBUS_REGS_PER,  0x000,  UIN+UIN_MODBUS_REGS_PER*0,},
    {0x31,  MB_FUNC_READ_INPUT_REGISTER,  UIN_MODBUS_REGS_PER,  0x000,  UIN+UIN_MODBUS_REGS_PER*1,},
    {0x32,  MB_FUNC_READ_INPUT_REGISTER,  UIN_MODBUS_REGS_PER,  0x000,  UIN+UIN_MODBUS_REGS_PER*2,},
    {0x33,  MB_FUNC_READ_INPUT_REGISTER,  UIN_MODBUS_REGS_PER,  0x000,  UIN+UIN_MODBUS_REGS_PER*3,},
    {0x34,  MB_FUNC_READ_INPUT_REGISTER,  UIN_MODBUS_REGS_PER,  0x000,  UIN+UIN_MODBUS_REGS_PER*4,},
    {0x35,  MB_FUNC_READ_INPUT_REGISTER,  UIN_MODBUS_REGS_PER,  0x000,  UIN+UIN_MODBUS_REGS_PER*5,},
    {0x36,  MB_FUNC_READ_INPUT_REGISTER,  UIN_MODBUS_REGS_PER,  0x000,  UIN+UIN_MODBUS_REGS_PER*6,},
    {0x37,  MB_FUNC_READ_INPUT_REGISTER,  UIN_MODBUS_REGS_PER,  0x000,  UIN+UIN_MODBUS_REGS_PER*7,},
    {0x38,  MB_FUNC_READ_INPUT_REGISTER,  UIN_MODBUS_REGS_PER,  0x000,  UIN+UIN_MODBUS_REGS_PER*8,},
    {0x39,  MB_FUNC_READ_INPUT_REGISTER,  UIN_MODBUS_REGS_PER,  0x000,  UIN+UIN_MODBUS_REGS_PER*9,},
    {0x3A,  MB_FUNC_READ_INPUT_REGISTER,  UIN_MODBUS_REGS_PER,  0x000,  UIN+UIN_MODBUS_REGS_PER*10,},
    {0x3B,  MB_FUNC_READ_INPUT_REGISTER,  UIN_MODBUS_REGS_PER,  0x000,  UIN+UIN_MODBUS_REGS_PER*11,},
    {0x3C,  MB_FUNC_READ_INPUT_REGISTER,  UIN_MODBUS_REGS_PER,  0x000,  UIN+UIN_MODBUS_REGS_PER*12,},
    {0x3D,  MB_FUNC_READ_INPUT_REGISTER,  UIN_MODBUS_REGS_PER,  0x000,  UIN+UIN_MODBUS_REGS_PER*13,},
    {0x3E,  MB_FUNC_READ_INPUT_REGISTER,  UIN_MODBUS_REGS_PER,  0x000,  UIN+UIN_MODBUS_REGS_PER*14,},
    {0x3F,  MB_FUNC_READ_INPUT_REGISTER,  UIN_MODBUS_REGS_PER,  0x000,  UIN+UIN_MODBUS_REGS_PER*15,},
    {0x40,  MB_FUNC_READ_INPUT_REGISTER,  UIN_MODBUS_REGS_PER,  0x000,  UIN+UIN_MODBUS_REGS_PER*16,},
    {0x41,  MB_FUNC_READ_INPUT_REGISTER,  UIN_MODBUS_REGS_PER,  0x000,  UIN+UIN_MODBUS_REGS_PER*17,},
    {0x42,  MB_FUNC_READ_INPUT_REGISTER,  UIN_MODBUS_REGS_PER,  0x000,  UIN+UIN_MODBUS_REGS_PER*18,},
    {0x43,  MB_FUNC_READ_INPUT_REGISTER,  UIN_MODBUS_REGS_PER,  0x000,  UIN+UIN_MODBUS_REGS_PER*19,},
    {0x44,  MB_FUNC_READ_INPUT_REGISTER,  UIN_MODBUS_REGS_PER,  0x000,  UIN+UIN_MODBUS_REGS_PER*20,},
    {0x45,  MB_FUNC_READ_INPUT_REGISTER,  UIN_MODBUS_REGS_PER,  0x000,  UIN+UIN_MODBUS_REGS_PER*21,},
    {0x46,  MB_FUNC_READ_INPUT_REGISTER,  UIN_MODBUS_REGS_PER,  0x000,  UIN+UIN_MODBUS_REGS_PER*22,},
    {0x47,  MB_FUNC_READ_INPUT_REGISTER,  UIN_MODBUS_REGS_PER,  0x000,  UIN+UIN_MODBUS_REGS_PER*23,},
    {0x48,  MB_FUNC_READ_INPUT_REGISTER,  UIN_MODBUS_REGS_PER,  0x000,  UIN+UIN_MODBUS_REGS_PER*24,},
    {0x49,  MB_FUNC_READ_INPUT_REGISTER,  UIN_MODBUS_REGS_PER,  0x000,  UIN+UIN_MODBUS_REGS_PER*25,},
    {0x4A,  MB_FUNC_READ_INPUT_REGISTER,  UIN_MODBUS_REGS_PER,  0x000,  UIN+UIN_MODBUS_REGS_PER*26,},
    {0x4B,  MB_FUNC_READ_INPUT_REGISTER,  UIN_MODBUS_REGS_PER,  0x000,  UIN+UIN_MODBUS_REGS_PER*27,},
    {0x4C,  MB_FUNC_READ_INPUT_REGISTER,  UIN_MODBUS_REGS_PER,  0x000,  UIN+UIN_MODBUS_REGS_PER*28,},
    {0x4D,  MB_FUNC_READ_INPUT_REGISTER,  UIN_MODBUS_REGS_PER,  0x000,  UIN+UIN_MODBUS_REGS_PER*29,},
    {0x4E,  MB_FUNC_READ_INPUT_REGISTER,  UIN_MODBUS_REGS_PER,  0x000,  UIN+UIN_MODBUS_REGS_PER*30,},
    {0x4F,  MB_FUNC_READ_INPUT_REGISTER,  UIN_MODBUS_REGS_PER,  0x000,  UIN+UIN_MODBUS_REGS_PER*31,},

    {0x50,  MB_FUNC_READ_CCHECK,  			IKP_MODBUS_REGS_PER,  0x000,  IKP+IKP_MODBUS_REGS_PER*0,},
    {0x51,  MB_FUNC_READ_CCHECK,  			IKP_MODBUS_REGS_PER,  0x000,  IKP+IKP_MODBUS_REGS_PER*1,},
    {0x52,  MB_FUNC_READ_CCHECK,  			IKP_MODBUS_REGS_PER,  0x000,  IKP+IKP_MODBUS_REGS_PER*2,},
    {0x53,  MB_FUNC_READ_CCHECK,  			IKP_MODBUS_REGS_PER,  0x000,  IKP+IKP_MODBUS_REGS_PER*3,},
    {0x54,  MB_FUNC_READ_CCHECK,  			IKP_MODBUS_REGS_PER,  0x000,  IKP+IKP_MODBUS_REGS_PER*4,},
    {0x55,  MB_FUNC_READ_CCHECK,  			IKP_MODBUS_REGS_PER,  0x000,  IKP+IKP_MODBUS_REGS_PER*5,},
    {0x56,  MB_FUNC_READ_CCHECK,  			IKP_MODBUS_REGS_PER,  0x000,  IKP+IKP_MODBUS_REGS_PER*6,},
    {0x57,  MB_FUNC_READ_CCHECK,  			IKP_MODBUS_REGS_PER,  0x000,  IKP+IKP_MODBUS_REGS_PER*7,},
    {0x58,  MB_FUNC_READ_CCHECK,  			IKP_MODBUS_REGS_PER,  0x000,  IKP+IKP_MODBUS_REGS_PER*8,},
    {0x59,  MB_FUNC_READ_CCHECK,  			IKP_MODBUS_REGS_PER,  0x000,  IKP+IKP_MODBUS_REGS_PER*9,},
    {0x5A,  MB_FUNC_READ_CCHECK,  			IKP_MODBUS_REGS_PER,  0x000,  IKP+IKP_MODBUS_REGS_PER*10,},
    {0x5B,  MB_FUNC_READ_CCHECK,  			IKP_MODBUS_REGS_PER,  0x000,  IKP+IKP_MODBUS_REGS_PER*11,},
    {0x5C,  MB_FUNC_READ_CCHECK,  			IKP_MODBUS_REGS_PER,  0x000,  IKP+IKP_MODBUS_REGS_PER*12,},
    {0x5D,  MB_FUNC_READ_CCHECK,  			IKP_MODBUS_REGS_PER,  0x000,  IKP+IKP_MODBUS_REGS_PER*13,},
    {0x5E,  MB_FUNC_READ_CCHECK,  			IKP_MODBUS_REGS_PER,  0x000,  IKP+IKP_MODBUS_REGS_PER*14,},
    {0x5F,  MB_FUNC_READ_CCHECK,  			IKP_MODBUS_REGS_PER,  0x000,  IKP+IKP_MODBUS_REGS_PER*15,},
};
uint8_t current_modbus_module;
uint8_t *modbus_request;
uint16_t modbus_modules_number(void)
{
	return sizeof(modbus_modules)/sizeof(struct modbus_modules_struct);
}
void modbus_modules_init(void)
{
    uint16_t i,j;
    for(i=0;i<sizeof(modbus_modules)/sizeof(struct modbus_modules_struct);++i)   /* Clear ModBus registers for inputs */
    {
        switch(modbus_modules[i].cmd)
        {
        case MB_FUNC_READ_COILS:
        case MB_FUNC_READ_DISCRETE_INPUTS:
        	for(j=0;j<modbus_modules[i].number;j+=16)
        	{
        		ModBus_regs[modbus_modules[i].to +(j/16)] = 0;
        	}
        	break;
        case MB_FUNC_READ_HOLDING_REGISTER:
        case MB_FUNC_READ_INPUT_REGISTER:
        case MB_FUNC_READ_CCHECK:
            for(j=0;j<modbus_modules[i].number;++j)
            {
                ModBus_regs[modbus_modules[i].to+j] = 0;
            }
            break;
        case MB_FUNC_WRITE_MULTIPLE_REGISTERS:
        	for(j=0;j<modbus_modules[i].number;++j)
        	{
        		ModBus_regs[modbus_modules[i].from+j] = 0;
        	}
        	break;
        }
    }
    current_modbus_module = 0;
    modbus_request = (uint8_t*)pvPortMalloc(MODBUS_REQUEST_LENGTH); /* Allocate space for buffer */
}
void next_modbus_module(void)
{
    if(++current_modbus_module >= sizeof(modbus_modules)/sizeof(struct modbus_modules_struct))
        current_modbus_module = 0;
}
void send_modbus_request(void)
{
    uint16_t size_of_request = 0;
    struct modbus_modules_struct *m = &modbus_modules[current_modbus_module];
    uint16_t tmp;

    modbus_request[size_of_request++] = m->address;
    modbus_request[size_of_request++] = m->cmd;
    switch(m->cmd)
    {
    case MB_FUNC_READ_COILS:
    case MB_FUNC_READ_DISCRETE_INPUTS:
        break;
    case MB_FUNC_READ_HOLDING_REGISTER:
    case MB_FUNC_READ_INPUT_REGISTER:
        modbus_request[size_of_request++] = (uint8_t)(m->from >> 8);
        modbus_request[size_of_request++] = (uint8_t)m->from;
        modbus_request[size_of_request++] = (uint8_t)(m->number >> 8);
        modbus_request[size_of_request++] = (uint8_t)m->number;
        break;
    case MB_FUNC_READ_CCHECK:
//    	modbus_request[size_of_request++] = 0;
    	modbus_request[size_of_request++] = (uint8_t)ModBus_regs[YEAR];
//    	modbus_request[size_of_request++] = 0;
    	modbus_request[size_of_request++] = (uint8_t)ModBus_regs[DAY_MONTH];
//    	modbus_request[size_of_request++] = 0;
    	modbus_request[size_of_request++] = (uint8_t)(ModBus_regs[DAY_MONTH] >> 8);
    	break;
    case MB_FUNC_WRITE_MULTIPLE_REGISTERS:
    	modbus_request[size_of_request++] = (uint8_t)(m->to >> 8);
    	modbus_request[size_of_request++] = (uint8_t)m->to;
    	modbus_request[size_of_request++] = (uint8_t)(m->number >> 8);
    	modbus_request[size_of_request++] = (uint8_t)m->number;
    	modbus_request[size_of_request++] = (uint8_t)(m->number*2);
    	modbus_request[size_of_request++] = (uint8_t)(ModBus_regs[modbus_modules[current_modbus_module].from]>>8);
    	modbus_request[size_of_request++] = (uint8_t)(ModBus_regs[modbus_modules[current_modbus_module].from]);
    	modbus_request[size_of_request++] = (uint8_t)(ModBus_regs[modbus_modules[current_modbus_module].from+1]>>8);
    	modbus_request[size_of_request++] = (uint8_t)(ModBus_regs[modbus_modules[current_modbus_module].from+1]);
    	break;
    default:
    	return;
    }
    //adding CRC16
    tmp=Crc16(modbus_request,size_of_request);
    modbus_request[size_of_request++] = (uint8_t)tmp;
    modbus_request[size_of_request++] = (uint8_t)(tmp >> 8);

    DCON_TE_ON;                                             /* Turn On The RS-485 Transmitter */
    HAL_UART_Transmit_IT(&hDCON_PORT, modbus_request,size_of_request);/* Send the message via RS-485 Transmitter */
}
uint16_t modbus_answer_process(uint8_t *buffer,uint16_t bytes)
{
    uint16_t tmp,ret,i;
    ret = pdFALSE;

    tmp=Crc16(buffer,(bytes-2));
    if((buffer[bytes-2]==(tmp&0x00FF)) & (buffer[bytes-1]==(tmp>>8)))
    {
        uint8_t *ptr;
        switch(buffer[1])
			{
            case MB_FUNC_READ_COILS:
            case MB_FUNC_READ_DISCRETE_INPUTS:
				ret = pdTRUE;
				break;
			case MB_FUNC_READ_HOLDING_REGISTER:
			case MB_FUNC_READ_INPUT_REGISTER:
			    ptr = &buffer[3];
			    for(i=0;i<modbus_modules[current_modbus_module].number;++i)
                {
                    ModBus_regs[modbus_modules[current_modbus_module].to + i] = (*ptr++ << 8);
                    ModBus_regs[modbus_modules[current_modbus_module].to + i] |= *ptr++;
                }
				ret = pdTRUE;
				break;
			case MB_FUNC_READ_CCHECK:
				ptr = &buffer[2];
				uint8_t *mb_ptr;
				mb_ptr = (uint8_t*)&ModBus_regs[modbus_modules[current_modbus_module].to];

				*mb_ptr++ = *(ptr+3);		/* Save ID */
				*mb_ptr++ = *(ptr+2);
				*mb_ptr++ = *(ptr+1);
				*mb_ptr++ = *ptr;
				ptr += 4;

				*mb_ptr++ = *(ptr+1);		/* Save DEEP */
				*mb_ptr++ = *ptr;
				ptr += 2;

				*mb_ptr++ = *(ptr+1);		/* Save SPEEDC */
				*mb_ptr++ = *ptr;
				ptr += 2;

				*mb_ptr++ = *(ptr++);		/* Save NEIC */
				*mb_ptr++ = 0;

				*mb_ptr++ = *(ptr++);		/* Save NEI */
				*mb_ptr++ = 0;

				*mb_ptr++ = *(ptr++);		/* Save TYPE */
				*mb_ptr++ = 0;

				*mb_ptr++ = *(ptr++);		/* Save YEARI */
				*mb_ptr++ = 0;

				*mb_ptr++ = *(ptr++);		/* Save MONTHI */
				*mb_ptr++ = 0;
				ret = pdTRUE;
				break;
			case MB_FUNC_WRITE_MULTIPLE_REGISTERS:
				if((buffer[3] == modbus_modules[current_modbus_module].to) && (buffer[5] == modbus_modules[current_modbus_module].number))
				{
					ret = pdTRUE;
				}
				ret = pdTRUE;
				break;
			case MB_FUNC_WRITE_SINGLE_COIL:
            case MB_FUNC_WRITE_REGISTER:
            default:
                ret = pdTRUE;
                break;
			}
    }
//    next_modbus_module();
    return ret;
}
void modbus_set_register(void)
{
//	if(modbus_modules[current_modbus_module].address <= 64)
		ModBus_regs[MODBUS_OVER_DCON_REGISTER +(modbus_modules[current_modbus_module].address)/16] |= (1<<((modbus_modules[current_modbus_module].address)%16));
}
void modbus_reset_register(void)
{
//	if(modbus_modules[current_modbus_module].address <= 64)
		ModBus_regs[MODBUS_OVER_DCON_REGISTER +(modbus_modules[current_modbus_module].address)/16] &= ~(1<<((modbus_modules[current_modbus_module].address)%16));
}
uint16_t modbus_current_module(void)
{
	return modbus_modules[current_modbus_module].address;
}
