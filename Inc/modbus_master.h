#ifndef MODBUS_MASTER_H_INCLUDED
#define MODBUS_MASTER_H_INCLUDED

#include "plc_hw.h"
#include "modbus_slave.h"



#define MODBUS_DCON_TIMER_QUANTUM   (100)
#define MODBUS_OVER_DCON_BAUDRATE   (19200UL)
#define MODBUS_REQUEST_LENGTH       (256)

uint16_t modbus_modules_number(void);
void modbus_modules_init(void);
void next_modbus_module(void);
void send_modbus_request(void);
uint16_t modbus_answer_process(uint8_t*,uint16_t);
void modbus_set_register(void);
void modbus_reset_register(void);
uint16_t modbus_current_module(void);
#endif /* MODBUS_MASTER_H_INCLUDED */
