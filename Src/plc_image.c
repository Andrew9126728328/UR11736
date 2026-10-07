/*
dtst - проверить дискрету на истинность
dset - установить дискрету в TRUE
dclr - установить дискрету в FALSE
!  - логическая НЕ
&& - логическая И
|| - логическая ИЛИ
tstart - запустить таймер (имя таймера), после старта таймера переменная_имя таймера переходит в TRUE, после истечения задержки - в FALSE
tstop - остановить таймер (имя таймера)
gstart - запустить генератор импульсов
*/
/*****************************************************/
#include "stm32f2xx_hal.h"
#include "cmsis_os.h"
#include "modbus_slave.h"
#include "plc.h"
#include "plc_image.h"
#include "discrete.h"
#include "ur11435_lib.h"


void null_function(void){};
uint16_t unsigned_comp16(uint16_t a,uint16_t b)
{
    if(a>b) return GREAT;
    if(a<b) return LESS;
    return EQUAL;
}
uint16_t signed_comp16(int16_t a,int16_t b)
{
    if(a>b) return GREAT;
    if(a<b) return LESS;
    return EQUAL;
}

/******************************* Тест ***************************************************/

void Test1_func(void)
{
}



