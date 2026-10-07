#ifndef PLC_IMAGE_H_INCLUDED
#define PLC_IMAGE_H_INCLUDED

#include "stm32f2xx_hal.h"

/******************** Variable type ******************************/
typedef struct{
    uint16_t    ndx;        /* Index in ModBus_regs */
    uint8_t     type;       /* Type of variable */
    uint8_t     prop;       /* Properties of variable */
}plc_var_t;
/******************** Signals in plc ******************************/
typedef const plc_var_t* signal_t;
/******************** Timer type ******************************/
typedef struct {
    const plc_var_t *var;       /* Pointer to variable */
    uint32_t        resp_time;  /* Response time (private) */
    uint32_t        value;      /* Operating hours timer */
}plc_timer_t;

/******************** Bind variable and signals type **********/
typedef struct{
    const plc_var_t *var;       /* Pointer to variable */
    uint16_t        cond;       /* What is checked? */
    uint16_t        limit;      /* Limit to check */
}signal_table_t;
/******************** Type of variable ********************/
enum {DISCRETE=0,ANALOG,TIMER,GENERATOR};
/******************** Definitions *************************/
enum {BIT_0=0,BIT_1,BIT_2,BIT_3,BIT_4,BIT_5,BIT_6,BIT_7,BIT_8,BIT_9,BIT_10,BIT_11,BIT_12,BIT_13,BIT_14,BIT_15}; /* Properties of discrete variable */
enum {SIGNED=0,UNSIGNED};       /* Properties of analog variable */
enum {EQUAL=0,GREAT,LESS};
enum {NONE=0,CHECK,UP,DOWN};    /* What is checked? */
/******************** Prototypes *************************/
#define VAR(name,ndx,type,prop)   const plc_var_t name = {(ndx),(type),(prop)}
#define EXTERN_VAR(name)          extern const plc_var_t name
#define SIGNAL(var,cond,limit)   {(&var),(cond),(limit)}

// Тип указателя на функцию для форматирования данных
typedef void (*fuction_reformat_t)(uint16_t,uint16_t);
typedef struct											/* Структура для движка форматированиея данных */
{
	uint16_t from;										/* Откуда взять (Modbus index) */
	uint16_t to;										/* Куда положить (Modbus index) */
	fuction_reformat_t reformat_function;				/* Какой функцией обработать */
}reformat_t;
extern const reformat_t reformat_table[];
extern const uint32_t sizeof_reformat_table;
extern void split(uint16_t from,uint16_t to);			/* Разделяет младший и старший байты на два регистра */
/******************** Operating time type ******************************/
typedef struct											/* Структура для движка форматированиея данных */
{
	const plc_var_t 	*var;
	uint32_t	bkp_reg;
	uint16_t	m_ndx;
}operating_time_t;
extern const operating_time_t operating_time_table[];
extern const uint32_t sizeof_operating_time_table;
void calculate_operating_time(void);
/****************************************************************************************************************/
#include "cmsis_os.h"
#include "sw_version.h"
#include "plc_var.h"
#include "plc_signals.h"
#include "plc_states.h"
#include "plc_timers.h"
#include "discrete.h"
#endif /* PLC_IMAGE_H_INCLUDED */
