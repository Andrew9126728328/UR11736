#ifndef PLC_VAR_H_INCLUDED
#define PLC_VAR_H_INCLUDED
#include "plc_image.h"
#include "modbus_slave.h"

/* ----------------------------------------------------------------- */
#define analog(x)	ModBus_regs[((plc_var_t*)x)->ndx]
/* ----------------------------------------------------------------- */
EXTERN_VAR(sig_reset);
EXTERN_VAR(sig_empty);

EXTERN_VAR(discrete_example);
EXTERN_VAR(analog_example);
EXTERN_VAR(timer_example);


#endif /* PLC_VAR_H_INCLUDED */
