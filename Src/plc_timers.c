#include "stm32f2xx_hal.h"
#include "cmsis_os.h"
#include "modbus_slave.h"
#include "plc.h"
#include "plc_image.h"
#include "discrete.h"

#define DEF_TIMER(var)   {(&var),0,0}

plc_timer_t timer_table[] =
{
	DEF_TIMER(timer_example),
};
const uint32_t sizeof_timer_table = sizeof(timer_table)/sizeof(plc_timer_t);
void gstart(const plc_var_t *timer,uint32_t value)
{
    uint16_t i;
    for(i=0;i<sizeof_timer_table;++i)
    {
    	if(timer_table[i].var == timer)		/* Find timer in table */
    	{
    		if(timer_table[i].var->type == GENERATOR)
    		{
    			timer_table[i].value = value;
    			timer_table[i].resp_time = HAL_GetTick() + timer_table[i].value;
    			break;
    		}
    	}
    }
}
void tstart(const plc_var_t *timer,uint32_t value)
{
    uint16_t i;
    for(i=0;i<sizeof_timer_table;++i)
    {
        if(timer_table[i].var == timer)		/* Find timer in table */
            break;
    }
    timer_table[i].value = value;
    timer_table[i].resp_time = HAL_GetTick() + timer_table[i].value;
    ModBus_regs[timer->ndx] |= (1<<timer->prop);
}
void tstop(const plc_var_t *timer)
{
    uint16_t i;
    for(i=0;i<sizeof_timer_table;++i)
    {
        if(timer_table[i].var == timer)
        {
        	ModBus_regs[timer->ndx] &= ~(1<<timer->prop);
        	break;
        }
    }
}
uint16_t ttest(const plc_var_t *timer)
{
    return dtst(timer);
}
uint32_t tgettime(const plc_var_t *timer)
{
	uint16_t i;
	uint32_t ret=0;
	if(dtst(timer))
	{
		for(i=0;i<sizeof_timer_table;++i)
		{
			if(timer_table[i].var == timer)
			{
				if(timer_table[i].resp_time >= HAL_GetTick())
				{
					ret = timer_table[i].resp_time - HAL_GetTick();
				}else
				{
					ret = timer_table[i].resp_time + (0xFFFFFFFF - HAL_GetTick()) + 1;
				}
			}
		}
	}
	return ret;
}
static signal_t signal;
void vPLC_TimersCallback(signed portBASE_TYPE *xHigherPriorityTaskWoken)
{
    uint16_t i;
    for(i=0;i<sizeof_timer_table;++i)
    {
        switch(timer_table[i].var->type)
        {
        case TIMER:
            if((ModBus_regs[timer_table[i].var->ndx] & (1<<timer_table[i].var->prop)) && (HAL_GetTick() == timer_table[i].resp_time))
            {
                ModBus_regs[timer_table[i].var->ndx] &= ~(1<<timer_table[i].var->prop);
                signal = timer_table[i].var;
                xQueueSendFromISR(xPLC_Queue,&signal,xHigherPriorityTaskWoken);
            }
            break;
        case GENERATOR:
            if(HAL_GetTick() == timer_table[i].resp_time)
            {
                ModBus_regs[timer_table[i].var->ndx] ^= (1<<timer_table[i].var->prop);
                timer_table[i].resp_time = HAL_GetTick() + timer_table[i].value;
                signal = timer_table[i].var;
                xQueueSendFromISR(xPLC_Queue,&signal,xHigherPriorityTaskWoken);
            }
            break;
        }
    }
}
