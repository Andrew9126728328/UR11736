#include "plc_image.h"

//#define DEBUG_1


#ifdef DEBUG_1


#endif

/********************** Высоковольтный выключатель  и возбуждение СД*********************/

const state_table_t TABLE_1[]=
{
/*  из состояния               по событию                выполнить                в состояние */

	{STATE_ANY,					&sig_reset,    				null_function,			STATE_NO_NEW},
};

/************************* Разгон *******************************/

const state_table_t TABLE_2[]=
{
	{STATE_ANY,					&sig_reset,    				null_function,			STATE_NO_NEW},
};


/********************************* Генераторы прямоугольных импульсов ********************************************/

const state_table_t GENERATOR_TABLE[]=    // генераторы
{
	{STATE_ANY,                 &sig_reset,        		null_function,        			STATE_NO_NEW},
	{STATE_ANY,                 &sig_reset,        		null_function,        			STATE_NO_NEW},
};

state_machine_t state_machines_table[] =
{
	{TABLE_1,                  	sizeof(TABLE_1)/sizeof(state_table_t),			STATE_NO_NEW},
	{TABLE_2,        			sizeof(TABLE_2)/sizeof(state_table_t),       	STATE_NO_NEW},
	{GENERATOR_TABLE,           sizeof(GENERATOR_TABLE)/sizeof(state_table_t),	STATE_NO_NEW},
};
const uint32_t sizeof_state_machines_table = sizeof(state_machines_table)/sizeof(state_machine_t);
