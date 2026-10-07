#ifndef PLC_H_INCLUDED
#define PLC_H_INCLUDED
#include "plc_image.h"


#define BUILD_MONTH_IS_JAN (__DATE__[0] == 'J' && __DATE__[1] == 'a' && __DATE__[2] == 'n')
#define BUILD_MONTH_IS_FEB (__DATE__[0] == 'F')
#define BUILD_MONTH_IS_MAR (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'r')
#define BUILD_MONTH_IS_APR (__DATE__[0] == 'A' && __DATE__[1] == 'p')
#define BUILD_MONTH_IS_MAY (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'y')
#define BUILD_MONTH_IS_JUN (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'n')
#define BUILD_MONTH_IS_JUL (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'l')
#define BUILD_MONTH_IS_AUG (__DATE__[0] == 'A' && __DATE__[1] == 'u')
#define BUILD_MONTH_IS_SEP (__DATE__[0] == 'S')
#define BUILD_MONTH_IS_OCT (__DATE__[0] == 'O')
#define BUILD_MONTH_IS_NOV (__DATE__[0] == 'N')
#define BUILD_MONTH_IS_DEC (__DATE__[0] == 'D')

#define BUILD_MONTH \
    ( \
        (BUILD_MONTH_IS_JAN) ? 1 : \
        (BUILD_MONTH_IS_FEB) ? 2 : \
        (BUILD_MONTH_IS_MAR) ? 3 : \
        (BUILD_MONTH_IS_APR) ? 4 : \
        (BUILD_MONTH_IS_MAY) ? 5 : \
        (BUILD_MONTH_IS_JUN) ? 6 : \
        (BUILD_MONTH_IS_JUL) ? 7 : \
        (BUILD_MONTH_IS_AUG) ? 8 : \
        (BUILD_MONTH_IS_SEP) ? 9 : \
        (BUILD_MONTH_IS_OCT) ? 10 : \
        (BUILD_MONTH_IS_NOV) ? 11 : \
        (BUILD_MONTH_IS_DEC) ? 12 : 0\
    )

#define BUILD_DATE ((__DATE__[4]>'0')?(((__DATE__[4]-'0')*10)+(__DATE__[5]-'0')):(__DATE__[5]-'0'))

#define BUILD_YEAR (((__DATE__[7]-'0')*1000)+((__DATE__[8]-'0')*100)+((__DATE__[9]-'0')*10)+(__DATE__[10]-'0'))

/* Bits of PLC SYS Register */
enum {USB_UP = 0, COM1_OK, COM2_OK,COM3_OK,CAN_OK,ETH_OK,MODBUS_SLAVE_UP,DCON_UP,MODBUS_OVER_DCON_UP,MB_NET_UP,SD_MOUNT_OK};

#define PLC_TIMER_QUANTUM       5
#define QUEUE_PLC_LENGTH        128
#define ITEM_SIZE_QUEUE_PLC     sizeof(uint32_t)


extern xQueueHandle          xPLC_Queue;

void PLC_init(void);
void PLC_Signals_Generate(xTimerHandle pxTimer);
void PLC_process_sequential(signal_t signal);
void calculate_operating_time(void);

uint16_t unsigned_comp16(uint16_t a,uint16_t b);
uint16_t signed_comp16(int16_t a,int16_t b);
#endif /* PLC_H_INCLUDED */
