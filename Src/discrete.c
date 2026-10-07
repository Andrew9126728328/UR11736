#include "stm32f2xx_hal.h"
#include "cmsis_os.h"
#include "modbus_slave.h"
#include "plc.h"
#include "discrete.h"

/* Discrete variable */
void dset(const plc_var_t *dscr)                  /* Set discrete variable */
{
    ModBus_regs[dscr->ndx] |= (1<<dscr->prop);
}
void dclr(const plc_var_t *dscr)             /* Clear discrete variable */
{
    ModBus_regs[dscr->ndx] &= ~(1<<dscr->prop);
}
uint16_t dtst(const plc_var_t *dscr)             /* Test discrete variable */
{
    return (ModBus_regs[dscr->ndx] & (1<<dscr->prop))? pdTRUE:pdFALSE;
}
void dnot(const plc_var_t *dscr)             /* Invert discrete variable */
{
    ModBus_regs[dscr->ndx] ^= (1<<dscr->prop);
}


/* Any variable */
void vset_bit  (const plc_var_t *var,uint16_t bit)
{
    ModBus_regs[var->ndx] |= (1<<bit);
}
void vclr_bit  (const plc_var_t *var,uint16_t bit)
{
    ModBus_regs[var->ndx] &= ~(1<<bit);
}
uint16_t vtst_bit  (const plc_var_t *var,uint16_t bit)
{
    return (ModBus_regs[var->ndx] & (1<<bit))? pdTRUE:pdFALSE;
}
void vnot_bit  (const plc_var_t *var,uint16_t bit)
{
    ModBus_regs[var->ndx] ^= (1<<bit);
}
