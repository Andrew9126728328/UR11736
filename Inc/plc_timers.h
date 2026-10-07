#ifndef PLC_TIMERS_H_INCLUDED
#define PLC_TIMERS_H_INCLUDED

#include "plc_image.h"

void gstart(const plc_var_t *timer,uint32_t value);
void tstart(const plc_var_t *,uint32_t value);
void tstop(const plc_var_t *);
uint16_t ttest(const plc_var_t *);
uint32_t tgettime(const plc_var_t *timer);
void vPLC_TimersCallback(signed portBASE_TYPE*);
#endif /* PLC_TIMERS_H_INCLUDED */
