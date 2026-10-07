#include "plc_image.h"
/* указание переменных, вырабатывающих сигналы для перехода между состояниями "по событию" в "plc_states.c" */
const signal_table_t signal_table[] =
{
    /********** Переключатели **********/
	SIGNAL(discrete_example,       	CHECK,  0),
};
const uint32_t sizeof_signal_table = sizeof(signal_table)/sizeof(signal_table_t);
