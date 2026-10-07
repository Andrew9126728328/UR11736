#ifndef DISCRETE_H_INCLUDED
#define DISCRETE_H_INCLUDED

#include "plc_image.h"

void 		dset  (const plc_var_t *); /* Set discrete variable */
void 		dclr  (const plc_var_t *); /* Clear discrete variable */
uint16_t 	dtst  (const plc_var_t *); /* Test discrete variable */
void 		dnot  (const plc_var_t *); /* Invert discrete variable */

void 		vset_bit  (const plc_var_t *,uint16_t); /* Set bit in any variable */
void 		vclr_bit  (const plc_var_t *,uint16_t); /* Clear bit in any variable */
uint16_t 	vtst_bit  (const plc_var_t *,uint16_t); /* Test bit in any variable */
void 		vnot_bit  (const plc_var_t *,uint16_t); /* Invert bit in any variable */
#endif /* DISCRETE_H_INCLUDED */
