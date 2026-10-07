#include "stm32f2xx_hal.h"
#include "rtc.h"
#include "cmsis_os.h"
#include "modbus_slave.h"
#include "plc.h"

extern xQueueHandle          xPLC_Queue;

static signal_t signal;

static void reformat_process(void);

void PLC_init(void)
{
    uint16_t i;
    for(i=0;i<MODBUS_REGS_NUM;++i) ModBus_regs_old[i] = 0;  /* clear array ModBus_regs_old */
    signal = &sig_reset;
    xQueueSend(xPLC_Queue,&signal,0);
  //  gstart();          // запустить все таймеры gstart (меандр). Формат gstart(имя, xxx); где xxx-период

}
/* Software Timer for PLC Process */
void PLC_Signals_Generate(xTimerHandle pxTimer)
{
    register uint16_t i,k;
    /* Test data changes, send signals */
    xSemaphoreTake( myMutexModbusSlaveHandle, portMAX_DELAY );             /* Guard ModBus Slave process */
    for(i=MODBUS_DI_BASE;i<(MODBUS_VAR_BASE+MODBUS_REGS_VAR);++i)
    {
        if(ModBus_regs[i] != ModBus_regs_old[i])                    /* If changed */
        {
        	for(k=0;k<sizeof_signal_table;++k)
            {
                if(i == signal_table[k].var->ndx)                   /* If signal present for this register */
                {
                    switch(signal_table[k].var->type)
                    {
                    case ANALOG:
                        switch(signal_table[k].cond)
                        {
                        case CHECK:
                            switch(signal_table[k].var->prop)
                            {
                            case SIGNED:
                                if(signed_comp16((int16_t) ModBus_regs[i],(int16_t) signal_table[k].limit) == EQUAL)
                                    signal = signal_table[k].var;
                                    xQueueSend(xPLC_Queue,&signal,0);
                                break;
                            case UNSIGNED:
                                if(unsigned_comp16(ModBus_regs[i],signal_table[k].limit) == EQUAL)
                                    signal = signal_table[k].var;
                                    xQueueSend(xPLC_Queue,&signal,0);
                                break;
                            }
                            break;
                        case UP:
                            switch(signal_table[k].var->prop)
                            {
                            case SIGNED:
                                if(signed_comp16((int16_t) ModBus_regs[i],(int16_t) signal_table[k].limit) == GREAT)
                                    signal = signal_table[k].var;
                                    xQueueSend(xPLC_Queue,&signal,0);
                                break;
                            case UNSIGNED:
                                if(unsigned_comp16(ModBus_regs[i],signal_table[k].limit) == GREAT)
                                    signal = signal_table[k].var;
                                    xQueueSend(xPLC_Queue,&signal,0);
                                break;
                            }
                            break;
                        case DOWN:
                            switch(signal_table[k].var->prop)
                            {
                            case SIGNED:
                                if(signed_comp16((int16_t) ModBus_regs[i],(int16_t) signal_table[k].limit) == LESS)
                                    signal = signal_table[k].var;
                                    xQueueSend(xPLC_Queue,&signal,0);
                                break;
                            case UNSIGNED:
                                if(unsigned_comp16(ModBus_regs[i],signal_table[k].limit) == LESS)
                                    signal = signal_table[k].var;
                                    xQueueSend(xPLC_Queue,&signal,0);
                                break;
                            }
                            break;
                        }
                        break;
                    case DISCRETE:
                        if((ModBus_regs[i] ^ ModBus_regs_old[i]) & (1<<signal_table[k].var->prop))
                        {
                            switch(signal_table[k].cond)
                            {
                             default:
                             case CHECK:
                                 signal = signal_table[k].var;
                                 xQueueSend(xPLC_Queue,&signal,0);
                                 break;
                             case UP:
                                 if(ModBus_regs[i] & (1<<signal_table[k].var->prop))
                                 {
                                    signal = signal_table[k].var;
                                    xQueueSend(xPLC_Queue,&signal,0);
                                 }
                                 break;
                            case DOWN:
                                 if(ModBus_regs_old[i] & (1<<signal_table[k].var->prop))
                                 {
                                    signal = signal_table[k].var;
                                    xQueueSend(xPLC_Queue,&signal,0);
                                 }
                                 break;
                            }
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
            ModBus_regs_old[i] = ModBus_regs[i];
        }
    }
    reformat_process();
    xSemaphoreGive(myMutexModbusSlaveHandle);                              /* Remove the guard ModBus Slave process */
}
void PLC_process_sequential(signal_t signal)
{
    uint16_t i,k;
    if(signal == &sig_empty) return;
    for(i=0;i<sizeof_state_machines_table;++i)
    {
        const state_table_t *thisTable = state_machines_table[i].state_table;
        state_machine_t *thisMachine = &state_machines_table[i];
        for(k=0;k<thisMachine->size_of_table;++k,++thisTable)
        {
            if((signal==thisTable->received_signal) && ((thisMachine->current_state==thisTable->current_state) || (thisTable->current_state==STATE_ANY)))
            {
                (*thisTable->execute_function)();
                if ( thisTable->next_state != STATE_NO_NEW )
                {
                    thisMachine->current_state = thisTable->next_state;
                };
            };
        }
    }
}
/* Вычисление наработки */
void calculate_operating_time(void)
{
	uint16_t i;
	for(i=0;i<sizeof_operating_time_table;++i)
	{
		if(dtst(operating_time_table[i].var))
		{
			uint32_t *ptr = (uint32_t*)&ModBus_regs[operating_time_table[i].m_ndx];
			*ptr = HAL_RTCEx_BKUPRead(&hrtc, operating_time_table[i].bkp_reg) + 1;
			HAL_RTCEx_BKUPWrite(&hrtc, operating_time_table[i].bkp_reg, *ptr);
		}
	}
}
/* Движок форматирования данных */
static void reformat_process(void)
{
	uint16_t i;
	for(i=0;i<sizeof_reformat_table;++i)
	{
		reformat_table[i].reformat_function(reformat_table[i].from,reformat_table[i].to);
	}
}
/* Разделяет младший и старший байты на два регистра */
void split(uint16_t from,uint16_t to)
{
	ModBus_regs[to] = ModBus_regs[from] & 0x00FF;
	ModBus_regs[++to] = ModBus_regs[from] >> 8;
}
