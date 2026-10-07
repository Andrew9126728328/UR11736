/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */     
#include "stm32f2xx_hal.h"
#include "cmsis_os.h"
#include "adc.h"
#include "crc.h"
#include "dac.h"
#include "dma.h"
#include "fatfs.h"
#include "iwdg.h"
#include "lwip.h"
#include "rtc.h"
#include "sdio.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"

#include "gpio.h"
#include "ur11435_lib.h"
#include "usbd_cdc_if.h"
#include "plc.h"
#include "plc_timers.h"
#include "modbus_slave.h"
#include "modbus_master.h"
#include "dcon.h"
#include "eeprom.h"
#include "lwip/api.h"
#include "stdio.h"

#define GPIO_PIN_INDEX(__GPIO_PIN_x__)   (uint8_t)	(((__GPIO_PIN_x__) == (GPIO_PIN_0))? 0U :\
                                               	   	 ((__GPIO_PIN_x__) == (GPIO_PIN_1))? 1U :\
                                               	   	 ((__GPIO_PIN_x__) == (GPIO_PIN_2))? 2U :\
                                               	   	 ((__GPIO_PIN_x__) == (GPIO_PIN_3))? 3U :\
                                               	   	 ((__GPIO_PIN_x__) == (GPIO_PIN_4))? 4U :\
                                               	   	 ((__GPIO_PIN_x__) == (GPIO_PIN_5))? 5U :\
                                               	   	 ((__GPIO_PIN_x__) == (GPIO_PIN_6))? 6U :\
                                               	   	 ((__GPIO_PIN_x__) == (GPIO_PIN_7))? 7U :\
                                               	   	 ((__GPIO_PIN_x__) == (GPIO_PIN_8))? 8U :\
                                               	   	 ((__GPIO_PIN_x__) == (GPIO_PIN_9))? 9U :\
                                               	   	 ((__GPIO_PIN_x__) == (GPIO_PIN_10))? 10U :\
                                               	   	 ((__GPIO_PIN_x__) == (GPIO_PIN_11))? 11U :\
                                               	   	 ((__GPIO_PIN_x__) == (GPIO_PIN_12))? 12U :\
                                               	   	 ((__GPIO_PIN_x__) == (GPIO_PIN_13))? 13U :\
                                               	   	 ((__GPIO_PIN_x__) == (GPIO_PIN_14))? 14U : 15U)
/* USER CODE END Includes */
extern __IO uint32_t uwTick;
/* Variables -----------------------------------------------------------------*/
config_t *config_running;
const config_t config_default = { 	{ 0,
									 DCON_BAUDRATE,		MODBUS_OVER_DCON_BAUDRATE,
									{JOISTICK_1_KU,		JOISTICK_2_KU,		JOISTICK_3_KU,		JOISTICK_4_KU},
									{JOISTICK_1_ZERO,	JOISTICK_2_ZERO,	JOISTICK_3_ZERO,	JOISTICK_4_ZERO},
									{IP_ADDR0,			IP_ADDR1,			IP_ADDR2,			IP_ADDR3},
									{NETMASK_ADDR0,		NETMASK_ADDR1,		NETMASK_ADDR2,		NETMASK_ADDR3},
									{GW_ADDR0,			GW_ADDR1,			GW_ADDR2,			GW_ADDR3},
									 0 } };
di_prepare_t di_prepare[] = {
		{DI_TIMER_BOUNCE,0,DI_OFF},
		{DI_TIMER_BOUNCE,0,DI_OFF},
		{DI_TIMER_BOUNCE,0,DI_OFF},
		{DI_TIMER_BOUNCE,0,DI_OFF},
		{DI_TIMER_BOUNCE,0,DI_OFF},
		{DI_TIMER_BOUNCE,0,DI_OFF},
		{DI_TIMER_BOUNCE,0,DI_OFF},
		{DI_TIMER_BOUNCE,0,DI_OFF},
		{DI_TIMER_BOUNCE,0,DI_OFF},
		{DI_TIMER_BOUNCE,0,DI_OFF},
		{DI_TIMER_BOUNCE,0,DI_OFF},
		{DI_TIMER_BOUNCE,0,DI_OFF},
		{DI_TIMER_BOUNCE,0,DI_OFF},
		{DI_TIMER_BOUNCE,0,DI_OFF},
		{DI_TIMER_BOUNCE,0,DI_OFF},
		{DI_TIMER_BOUNCE,0,DI_OFF},
};

xTaskHandle StartTaskHandle;
xTaskHandle myTaskUSBHandle;
xTaskHandle Task_ModbusSlaveHandle;
xTaskHandle myTaskCOM3Handle;
xTaskHandle myTaskEthHandle;
xTaskHandle myTaskADCHandle;
xTaskHandle myTaskDACHandle;
xTaskHandle myTaskEEPROMHandle;
xTaskHandle myTaskSDCardHandle;
xTaskHandle myTaskPLCHandle;
xTaskHandle myTaskDIHandle;

//xQueueHandle myQueueCOM2Handle;
xQueueHandle myQueueSDCardHandle;
xQueueHandle xPLC_Queue;
#ifdef CONSOLE
xQueueHandle myQueueUSB_console;
#endif
xSemaphoreHandle myMutexModbusSlaveHandle;

xSemaphoreHandle myBinarySemCOM1_breakHandle;
xSemaphoreHandle myBinarySemCOM3Handle;
xSemaphoreHandle myBinarySemModBusOverDCONHandle;
xSemaphoreHandle myBinarySemUSBHandle;
xSemaphoreHandle myBinarySemModBusSlaveInWorkHandle;

enum {LED_TIMER_ID = 1,MODBUS_DCON_ID,PLC_TIMER_ID,PLC_TIMERS_ID,DIO_TIMER_ID,ADC_TIMER_ID,RTC_TIMER_ID};         /* ID for Timers */

xTimerHandle myTimerModbusOverDCONHandle;
xTimerHandle myTimerLEDHandle;
xTimerHandle myTimerADCHandle;
xTimerHandle myTimerDIHandle;
xTimerHandle myTimerRTCHandle;
xTimerHandle xPLC_Timer,xPLC_Timers;
/* USER CODE BEGIN Variables */
static uint16_t ADC_raw[ADC_SAMPLE_NUMBER][ADC_CHANELS_NUMBER];
/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartDefaultTask(void const * argument);
void StartTaskUSB_Console(void const * argument);
void StartTaskUSB(void const * argument);
void StartTaskModBusSlaveCOM(void const * argument);
void StartTaskModBusMasterCOM(void const * argument);
void StartTaskEth(void const * argument);
void StartTaskEEPROM(void const * argument);
void StartTaskSDCard(void const * argument);
void StartTaskPLC(void const * argument);
void StartTaskDI(void const * argument);
void CallbackLED(xTimerHandle pxTimer);
void CallbackModbusOverDCONTimer(xTimerHandle pxTimer);
void CallbackADCimer(xTimerHandle pxTimer);
void CallbackDIOTimer(xTimerHandle pxTimer);
void CallbackRTCTimer(xTimerHandle pxTimer);

extern void MX_USB_DEVICE_Init(void);
extern void MX_FATFS_Init(void);
extern void MX_LWIP_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */


void SDlog(char *message,uint32_t value,uint16_t level)
{
#ifdef LOG_ENABLE
	log_record_t log_record;

//	if(eTaskGetState(myTaskSDCardHandle) < eSuspended)
//	{
		log_record.hours_minutes = ModBus_regs[HOURS_MINUTES];
		log_record.seconds = ModBus_regs[SECONDS];
		log_record.message = (uint8_t*)message;
		log_record.value = value;
		log_record.level = level;
		xQueueSend( myQueueSDCardHandle, ( void * ) &log_record, ( TickType_t ) 0 );
//	}
#endif /* LOG_ENABLE */
}

/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* Hook prototypes */
void vApplicationIdleHook(void);

/* USER CODE BEGIN 2 */
#define _GET_DIFF(a,b)      (a>b)?(a-b):(b-a)
//extern uint16_t ModBus_regs[];
void vApplicationIdleHook( void )
{
   /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
   to 1 in FreeRTOSConfig.h. It will be called on each iteration of the idle
   task. It is essential that code added to this hook function never attempts
   to block in any way (for example, call xQueueReceive() with a block time
   specified, or call vTaskDelay()). If the application makes use of the
   vTaskDelete() API function (as this demo application does) then it is also
   important that vApplicationIdleHook() is permitted to return to its calling
   function, because it is the responsibility of the idle task to clean up
   memory allocated by the kernel to any task that has since been deleted. */
    static portTickType LastTick;
    static portTickType counter;             //наш трудяга счетчик
    static portTickType max_count ;        //максимальное значение счетчика, вычисляется при калибровке и соответствует 100% CPU idle
    uint32_t *plc_uptime = (uint32_t*)&ModBus_regs[PLC_UPTIME];

    counter++;                                                  //приращение счетчика

    if((LastTick + 250) < xTaskGetTickCount())    { //если прошло 250 тиков (250 мсек для моей платфрмы)
        LastTick = xTaskGetTickCount();
        if(counter > max_count) max_count = counter;          //это калибровка
        ModBus_regs[PLC_LOAD] = 1000 * (max_count - counter) / max_count;        //вычисляем текущую загрузку
        counter = 0;                                           //обнуляем счетчик
    }
    if(*plc_uptime < 7200) HAL_IWDG_Refresh(&hiwdg);				/* Reset & Reload */
}
/* USER CODE END 2 */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* Create the thread(s) */
  /* definition and creation of StartTask */
  if (xTaskCreate((TaskFunction_t)StartDefaultTask,"StartTask",configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY+1,&StartTaskHandle) != pdPASS)
  {
    led_on(RED);for(;;);
  }
  /* Create the queue(s) */

  /* definition and creation of myQueueSDCard */
  myQueueSDCardHandle = xQueueCreate( QUEUE_SDCard_LENGTH, sizeof(log_record_t));

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}
/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{
	vTaskDelay(300);
	MX_RTC_Init();
	HAL_RTCEx_DeactivateCalibrationOutPut(&hrtc);
	MX_DMA_Init();
	MX_ADC1_Init();
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)ADC_raw, ADC_CHANELS_NUMBER*ADC_SAMPLE_NUMBER);
	HAL_IWDG_Refresh(&hiwdg);
//	MX_IWDG_Init(IWDG_PRESCALER_16);
	eeprom.ee_initialize(SPI1,GPIOE,GPIO_PIN_1);
	/* definition and creation of myMutexModbusSlave */
	myMutexModbusSlaveHandle = xSemaphoreCreateMutex();
	/* definition and creation of myBinarySemModBusSlaveInWork */
	myBinarySemModBusSlaveInWorkHandle = xSemaphoreCreateBinary();
	/* definition and creation of myTimerLED */
	myTimerLEDHandle = xTimerCreate("LED_Timer",5,pdTRUE,( void * ) LED_TIMER_ID, CallbackLED);
	xTimerStop(myTimerLEDHandle, 0);
	/* definition and creation of myTimerADC */

	myTimerADCHandle = xTimerCreate("ADC_Timer",ADC_TIMER_QUANTUM,pdTRUE,( void * ) ADC_TIMER_ID, CallbackADCimer);
	xTimerStart( myTimerADCHandle, 0);
	/* definition and creation of myTimerDI */
	myTimerDIHandle = xTimerCreate("DI_Timer",DIO_TIMER_QUANTUM,pdTRUE,( void * ) DIO_TIMER_ID, CallbackDIOTimer);
	xTimerStart( myTimerDIHandle, 0);

	/* definition and creation of myTimerRTC */
	myTimerRTCHandle = xTimerCreate("RTC_Timer",RTC_TIMER_QUANTUM,pdTRUE,( void * ) RTC_TIMER_ID, CallbackRTCTimer);
	xTimerStart( myTimerRTCHandle, 0);

	ModBus_regs       = (uint16_t*)pvPortMalloc(MODBUS_REGS_NUM*sizeof(uint16_t));
	ModBus_regs_old   = (uint16_t*)pvPortMalloc(MODBUS_REGS_NUM*sizeof(uint16_t));
	memset(ModBus_regs,0,MODBUS_REGS_NUM*sizeof(uint16_t));
	memset(ModBus_regs_old,0,MODBUS_REGS_NUM*sizeof(uint16_t));

	CallbackRTCTimer(myTimerRTCHandle);
	SDlog("Питание включено",0,LSTART);
  /* USER CODE BEGIN StartDefaultTask */

  config_running = (config_t*)&ModBus_regs[CONFIG];			/* Allocate config */
  eeprom.ee_read(ADDRESS_CONFIG_IN_EEPROM,sizeof(config_running->data),config_running->data); /* Read config from eeprom */
  if(config_running->cfg.crc != eeprom.ee_crc_calculate(config_running))						/* Check CRC */
  {
	  uint16_t i;
	  led_on(RED);
	  for(i=0;i<sizeof(config_default.data);++i)
	  {
		  config_running->data[i] = config_default.data[i];									/* Copy default config into running config */
	  }
	  config_running->cfg.crc = eeprom.ee_crc_calculate(config_running);						/* Calculate CRC */
	  eeprom.ee_write(ADDRESS_CONFIG_IN_EEPROM,sizeof(config_running->data),config_running->data); /* Save config into eeprom */
	  SDlog("Конфигурация по умолчанию",i,LERROR);
  }

  ModBus_regs[PLC_HW_VER] = PLC_HW_VERSION;
  ModBus_regs[PLC_SW_VER] = PLC_SW_VERSION;
  ModBus_regs[PLC_SW_DATE] = (BUILD_DATE<<8) + BUILD_MONTH;
  ModBus_regs[PLC_SW_YEAR] = BUILD_YEAR;
  ModBus_regs[PLC_UPTIME] = 0;
  ModBus_regs[PLC_UPTIME_HI] = 0;
  /* definition and creation of myTaskUSB */
#ifdef CONSOLE
  if (xTaskCreate((TaskFunction_t)StartTaskUSB_Console,"USBTask",configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY+4,&myTaskUSBHandle) != pdPASS)
  {
    led_on(RED);for(;;);
  }
#else
  if (xTaskCreate((TaskFunction_t)StartTaskUSB,"USBTask",configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY+4,&myTaskUSBHandle) != pdPASS)
  {
    led_on(RED);for(;;);
  }
#endif /* CONSOLE */
  /* definition and creation of myTaskCOM1 */
/*  if (xTaskCreate((TaskFunction_t)StartTaskModBusSlaveCOM,"MSlaveTask",configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY+4,&myTaskCOM1Handle) != pdPASS)
  {
    led_on(RED);for(;;);
  }
*/
  /* definition and creation of myTaskCOM3 */
  if (xTaskCreate((TaskFunction_t)StartTaskModBusMasterCOM,"MMasterTask",configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY+4,&myTaskCOM3Handle) != pdPASS)
  {
    led_on(RED);for(;;);
  }

  /* definition and creation of myTaskEth */
  if (xTaskCreate((TaskFunction_t)StartTaskEth,"ETHTask",configMINIMAL_STACK_SIZE*8,NULL,tskIDLE_PRIORITY+3,&myTaskEthHandle) != pdPASS)
  {
    led_on(RED);for(;;);
  }

  /* definition and creation of myTaskEEPROM */
  if (xTaskCreate((TaskFunction_t)StartTaskEEPROM,"EEPOMTask",configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY+2,&myTaskEEPROMHandle) != pdPASS)
  {
    led_on(RED);for(;;);
  }

  /* definition and creation of myTaskSDCard */
#ifdef LOG_ENABLE
  if (xTaskCreate((TaskFunction_t)StartTaskSDCard,"SDCardTask",configMINIMAL_STACK_SIZE*32,NULL,tskIDLE_PRIORITY+1,&myTaskSDCardHandle) != pdPASS)
  {
    led_on(RED);for(;;);
  }
#endif /* LOG_ENABLE */
  /* definition and creation of myTaskPLC */
/*  if (xTaskCreate((TaskFunction_t)StartTaskPLC,"PLCTask",configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY+2,&myTaskPLCHandle) != pdPASS)
  {
    led_on(RED);for(;;);
  }
*/
  /* Infinite loop */
  for(;;)
  {
	  ModBus_regs[TASK_NUMBER] = uxTaskGetNumberOfTasks();
	  ModBus_regs[HEAP_FREE] = xPortGetFreeHeapSize();

      if(xSemaphoreTake(myBinarySemModBusSlaveInWorkHandle,500) == pdTRUE)
      {
    	  led_spark(RED,50);     /* Display "Device connect to USB Host" */
    	  //vTaskDelay(100);
      }
      else
      {
    	  led_spark(GREEN,250);     /* Display "Device in Job" */
      }
  }
  /* USER CODE END StartDefaultTask */
}

/* StartTaskUSB function */
#ifdef CONSOLE
void StartTaskUSB_Console(void const * argument)
{
    buffer8_t *pUSB_RxTx;
    myQueueUSB_console = xQueueCreate( 16, sizeof(buffer8_t*) );
    /* init code for USB_DEVICE */
  	MX_USB_DEVICE_Init();
	vTaskDelay(150);
	HAL_GPIO_WritePin(GPIOA, USB_LINE_UP_Pin, GPIO_PIN_SET);
	SDlog("USB консоль в работе",0,LSTART);
	vTaskDelay(2000);
  /* Infinite loop */
  for(;;)
  {
	  xQueueReceive(myQueueUSB_console,&(pUSB_RxTx),portMAX_DELAY);
	  CDC_Transmit_FS(pUSB_RxTx->buffer,pUSB_RxTx->counter);
  }
}
#else
void StartTaskUSB(void const * argument)
{
  /* USER CODE BEGIN StartTaskUSB */

    USB_RxTx.buffer = (uint8_t*)pvPortMalloc(USB_BUF_SIZE);             /* Allocate space for buffer */
    USB_RxTx.counter = 0;
    /* definition and creation of myBinarySemUSB */
    myBinarySemUSBHandle = xSemaphoreCreateBinary();
    /* Timer 7 config for brek time */
    MX_TIM7_Init(20000,MODBUS_BREAKTIME);
    usb_stop_break();
  /* init code for USB_DEVICE */
	MX_USB_DEVICE_Init();
	GPIOA->PUPDR |= (GPIO_NOPULL << GPIO_PIN_INDEX(GPIO_PIN_12)*2);	/* USB DP Free */
	vTaskDelay(150);
	GPIOA->PUPDR |= (GPIO_PULLUP << GPIO_PIN_INDEX(GPIO_PIN_12)*2);	/* USB DP Up */
	//HAL_GPIO_WritePin(GPIOA, USB_LINE_UP_Pin, GPIO_PIN_SET);
	SDlog("USB Модбас в работе",0,LSYSEVENT);
  /* Infinite loop */
  for(;;)
  {
	  if(xSemaphoreTake( myBinarySemUSBHandle, 500 ) == pdTRUE)
      {
		  xSemaphoreTake( myMutexModbusSlaveHandle, portMAX_DELAY );           	/* Guard ModBus Slave process */
    	  MODBUS_SLAVE(&USB_RxTx,MODBUS_RTU);                                            	/* Send ModBus Request to ModBus Process */
    	  CDC_Transmit_FS(USB_RxTx.buffer,USB_RxTx.counter);         			/* Send ModBus answer over USB Virt port */
    	  xSemaphoreGive(myMutexModbusSlaveHandle);                            /* Remove the guard ModBus Slave process */
    	  USB_RxTx.counter = 0;
    	  //xSemaphoreGive( myBinarySemModBusSlaveInWorkHandle );
    	  ModBus_regs[PLC_SYS_REG] |= (1<<USB_UP);
      }else /* Timeout */
	  {
    	  ModBus_regs[PLC_SYS_REG] &= ~(1<<USB_UP);
	  }
  }
  /* USER CODE END StartTaskUSB */
}
#endif /* Console */
void CallbackModbusOverDCONTimer(xTimerHandle pxTimer)
{
	xSemaphoreGive(myBinarySemModBusOverDCONHandle);
}
/* StartTaskModBusMasterCOM function */
void StartTaskModBusMasterCOM(void const * argument)
{
  /* USER CODE BEGIN StartTaskCOM3 */
	  /* definition and creation of myBinarySemCOM3 */
	  myBinarySemCOM3Handle = xSemaphoreCreateBinary();
  com3_RxTx.buffer = (uint8_t*)pvPortMalloc(MODBUS_BUF_SZ);           /* Allocate space for buffer */
  com3_RxTx.counter = 0;

  MX_TIM9_Init(((config_running->cfg.COM2_high_baudrate > 19200)? 20000:config_running->cfg.COM2_high_baudrate),MODBUS_BREAKTIME);
  MX_USART2_UART_Init(config_running->cfg.COM2_high_baudrate,UART_WORDLENGTH_8B,UART_STOPBITS_1,UART_PARITY_NONE,UART_MODE_TX_RX);
  vTaskDelay(15);
  if(dcon_modules_number())
  {
	  dcon_init();
  }
  else if(!modbus_modules_number())
  {
	  vTaskSuspend( NULL );  /* This task is no longer needed */
  }
  if(modbus_modules_number())
  {
	  modbus_modules_init();
	  myBinarySemModBusOverDCONHandle = xSemaphoreCreateBinary();
	  /* definition and creation of myTimerModbusOverDCON */
	  myTimerModbusOverDCONHandle = xTimerCreate("ModbusOverDCON_Timer",MODBUS_DCON_TIMER_QUANTUM,pdTRUE,( void * ) MODBUS_DCON_ID,CallbackModbusOverDCONTimer);
	  xSemaphoreTake (myBinarySemModBusOverDCONHandle, 0);
	  xTimerStart( myTimerModbusOverDCONHandle, 0);
  }
  xSemaphoreTake(myBinarySemCOM3Handle,0);
//  ModBus_regs[DCON_REGISTER] = ModBus_regs[DCON_REGISTER_HI] = ModBus_regs[DCON_ERRORS] = ModBus_regs[DCON_TIMEOUTS] = 0;
//  ModBus_regs[MODBUS_OVER_DCON_REGISTER] = ModBus_regs[MODBUS_OVER_DCON_REGISTER+1] = ModBus_regs[MODBUS_OVER_DCON_REGISTER+2]= ModBus_regs[MODBUS_OVER_DCON_REGISTER+3] =0;
//  ModBus_regs[MODBUS_OVER_DCON_ERRORS] = ModBus_regs[MODBUS_OVER_DCON_TIMEOUTS] = 0;
  /* Infinite loop */
/*  for(;;)
  {
	  uint8_t byte = 0x55;
	  vTaskDelay(20);DCON_TE_ON;HAL_UART_Transmit(&hDCON_PORT, &byte, 1, 10);
	  vTaskDelay(20);DCON_TE_OFF;
  }
*/  ModBus_regs[PLC_SYS_REG] |= (1<<COM3_OK);
	SDlog("Модбас мастер в работе",config_running->cfg.COM2_high_baudrate,LSYSEVENT);
	for(;;)
  {
	  vTaskDelay(5);
	  if((xSemaphoreTake (myBinarySemModBusOverDCONHandle, 0) == pdTRUE) && modbus_modules_number())
	  {

//          dcon_host_OK_request();
          vTaskDelay(5);
          /* Set new USART Mode */
          MX_TIM9_Init(((config_running->cfg.COM2_low_baudrate > 19200)? 20000:config_running->cfg.COM2_low_baudrate),MODBUS_BREAKTIME);
          MX_USART2_UART_Init(config_running->cfg.COM2_low_baudrate,UART_WORDLENGTH_8B,UART_STOPBITS_1,UART_PARITY_NONE,UART_MODE_TX_RX);
          com3_Mode = MODBUS_MODE;
          send_modbus_request();
          if(xSemaphoreTake(myBinarySemCOM3Handle,80) == pdTRUE)
          {
        	  if(modbus_answer_process(com3_RxTx.buffer,com3_RxTx.counter) == pdTRUE)
               {
        		  modbus_set_register();
               }
               else
               {
            	   modbus_reset_register();
            	   SDlog("Модбас мастер ошибка",modbus_current_module(),LNETERROR);
               }
          }
          else
          {
        	  modbus_reset_register();
        	  SDlog("Модбас мастер таймаут",modbus_current_module(),LNETERROR);
          }
           next_modbus_module();
          /* Restore USART DCON Mode */
           MX_TIM9_Init(((config_running->cfg.COM2_high_baudrate > 19200)? 20000:config_running->cfg.COM2_high_baudrate),MODBUS_BREAKTIME);
           MX_USART2_UART_Init(config_running->cfg.COM2_high_baudrate,UART_WORDLENGTH_8B,UART_STOPBITS_1,UART_PARITY_NONE,UART_MODE_TX_RX);
          com3_Mode = DCON_MODE;
          vTaskDelay(5);
#ifdef DEBUG_MAIN
          ++ModBus_regs[PLC_TPDO3_01];
#endif /* DEBUG_MAIN */
	  }
	  /* DCON Read Digital inputs */
	  if(dcon_modules_number())
	  {
	       dcon_dio_read_inputs();
	       HAL_UART_Receive_IT(&hDCON_PORT, &com3_RxByte, 1);                           /* Accept one byte only */
	       if(xSemaphoreTake(myBinarySemCOM3Handle,DCON_TIMEOUT) == pdTRUE)        /* Wait DCON Answer */
	       {
	    	   ModBus_regs[PLC_SYS_REG] |= (1<<DCON_UP);
	    	   ModBus_regs[PLC_SYS_REG] |= (1<<COM3_OK);
	            if((com3_RxTx.buffer[0] != '?') && (com3_RxTx.counter >=6))
	            {
	            	xSemaphoreTake( myMutexModbusSlaveHandle, portMAX_DELAY );             /* Guard ModBus Slave process */
	                dcon_write_inputs_to_modbus_reg(com3_RxTx.buffer);              /* Write Received DCON data to ModBus process */
	                xSemaphoreGive(myMutexModbusSlaveHandle);                              /* Remove the guard ModBus Slave process */
	            }
	            else
	            {
	            	SDlog("DCON мастер ошибка",dcon_current_vodule('r'),LNETERROR);
//	                ++ModBus_regs[DCON_ERRORS];                                 /* Wrong DCON answer Indication */
	            }
	       }
	       else                                                                     /* TimeOut DCON answer Indication*/
	       {
	            dcon_reset_register('r');
	            ModBus_regs[PLC_SYS_REG] &= ~(1<<COM3_OK);
//	            ++ModBus_regs[DCON_TIMEOUTS];
	            ModBus_regs[PLC_SYS_REG] &= ~(1<<DCON_UP);
//	            SDlog("DCON мастер таймаут",dcon_current_vodule('r'),LNETERROR);
	       }
	       /* Go to next Digital inputs module */
	       dcon_next_inputs_module();
	       /* DCON Write Digital outputs */
	       uint16_t i;
	       for(i=0;i<dcon_modules_number();++i)
	       {
	           if(dcon_write_check(i) == pdTRUE)
	           {
	        	   vTaskDelay(2);
	        	   dcon_dio_write_outputs();
	                HAL_UART_Receive_IT(&hDCON_PORT, &com3_RxByte, 1);                           /* Accept one byte only */
	                if(xSemaphoreTake(myBinarySemCOM3Handle,DCON_TIMEOUT) == pdTRUE)        /* Wait DCON Answer */
	                {
	                	ModBus_regs[PLC_SYS_REG] |= (1<<COM3_OK);
	                	if(com3_RxTx.buffer[0] == '>')
	                    {
	                        dcon_set_register('w');
	                    }
	                    else
	                    {
	                    	SDlog("DCON мастер ошибка",dcon_current_vodule('w'),LNETERROR);
//	                        ++ModBus_regs[DCON_ERRORS];                                  /* Wrong DCON answer Indication */
	                    }
	                }
	                else                                                                     /* TimeOut DCON answer Indication*/
	                {
	                    dcon_reset_register('w');
	                    ModBus_regs[PLC_SYS_REG] &= ~(1<<COM3_OK);
//	                    ++ModBus_regs[DCON_TIMEOUTS];
	                    SDlog("DCON мастер таймаут",dcon_current_vodule('w'),LNETERROR);
	                }
	           }
	       }
	  }
  }
  /* USER CODE END StartTaskCOM3 */
}

/* StartTaskEth function */
extern struct netif gnetif;
extern ETH_HandleTypeDef heth;
void StartTaskETH_LinkCheck(void const * argument)
{
	uint32_t phyreg;

	for(;;)
	{
		vTaskDelay(TIME_WAITING_ETH_LINK);
		if ((HAL_ETH_ReadPHYRegister(&heth, PHY_BSR, &phyreg)) != HAL_OK)		/* PHY Is Failure */
		{
			HAL_GPIO_WritePin(ETH_RST_GPIO_Port, ETH_RST_Pin, GPIO_PIN_RESET);	/* Reset PHY */
			vTaskDelay(1);
			HAL_GPIO_WritePin(ETH_RST_GPIO_Port, ETH_RST_Pin, GPIO_PIN_SET);
			vTaskDelay(1);
			/* Put the PHY in reset mode */
			if((HAL_ETH_WritePHYRegister(&heth, PHY_BCR, PHY_RESET)) == HAL_OK)
			{
				netif_set_link_up(&gnetif);									/* Set Logical Link Up */
			}
			else
			{
				netif_set_link_down(&gnetif);									/* Set Logical Link Down */
			}
		}
		else if(!(phyreg & PHY_LINKED_STATUS))									/* PHY Is Good, HW Link Is Down */
		{
			netif_set_link_down(&gnetif);										/* Set Logical Link Down */
		}
		else if(!netif_is_link_up(&gnetif))										/* PHY Is Good, HW Link Is Up, Logical Link Is Down */
		{
			netif_set_link_up(&gnetif);										/* Set Logical Link Up */
		}
	}
}
void StartTaskTCPconnect(void const * argument)
{
	struct netconn   *conn;
	struct netbuf   *inbuf;
	uint8_t* buf;
	u16_t buflen;

	conn = (struct netconn*)argument;

	while(1)
	{
		netconn_set_recvtimeout(conn,5000);

		if (netconn_recv(conn, &inbuf) == ERR_OK)   //приняли посылку
		{
			netbuf_data(inbuf, (void**)&buf, &buflen);
			Eth_RxTx.buffer = buf + 6;
			Eth_RxTx.counter = buflen - 6;
			MODBUS_SLAVE(&Eth_RxTx,MODBUS_TCP);
			buflen = Eth_RxTx.counter + 6;
			*(buf+5) = Eth_RxTx.counter;
			netconn_write(conn, buf, (size_t)buflen, NETCONN_NOCOPY);
			netbuf_delete(inbuf);
		}
		else
		{
			netbuf_delete(inbuf);
			netconn_delete(conn);
			vTaskDelete(NULL);
		}
	}
}
void StartTaskEth(void const * argument)
{
	struct netconn *conn;
	err_t err;

	/* USER CODE BEGIN StartTaskEth */
	vTaskDelay(5000);
	/* Reset PHY LAN8720 Up */
	HAL_GPIO_WritePin(ETH_RST_GPIO_Port, ETH_RST_Pin, GPIO_PIN_SET);
	vTaskDelay(1);
	/* init code for LWIP */
	MX_LWIP_Init();
	MX_IWDG_Init(IWDG_PRESCALER_4);
	xTaskCreate((TaskFunction_t)StartTaskETH_LinkCheck,"EthLinkCheck",configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY+1,NULL);

	/* Create a new TCP connection handle */
	conn = netconn_new(NETCONN_TCP);
	if (conn!= NULL)
	{
	    /* Bind to port 502 (Modbus/TCP) with default IP address */
	    err = netconn_bind(conn, NULL, 502);
	    if (err == ERR_OK)
	    {
	          /* Put the connection into LISTEN state */
	          netconn_listen(conn);
	          for(;;)
	          {
	        	  struct netconn *newconn = NULL;

	        	  ModBus_regs[PLC_SYS_REG] &= ~(1<<ETH_OK);
	        	  /* accept any icoming connection */
	        	  err = netconn_accept(conn,&newconn);
	        	  if(err == ERR_OK)
	        	  {
	        		  xTaskCreate((TaskFunction_t)StartTaskTCPconnect,"TCPconnect",configMINIMAL_STACK_SIZE*2,newconn, tskIDLE_PRIORITY+3,NULL);
	        	  }
	        	  else
	        	  {
	        		  netconn_close(newconn);
	        		  netconn_delete(newconn);
	        	  }
	          }
	    }else	/* can not bind netconn */
	    {

	    }

	}else 		/* can not create netconn */
	{

	}
	netconn_delete(conn);
	vTaskSuspend( NULL );  /* This task is no longer needed */
  /* USER CODE END StartTaskEth */
}
void S_StartTaskEth(void const * argument)
{
	struct netconn *conn;
	err_t err;
	char str[32];

	/* USER CODE BEGIN StartTaskEth */
	//vTaskDelay(10000);
	vTaskDelay(3000);
	/* Reset PHY LAN8720 Up */
	HAL_GPIO_WritePin(ETH_RST_GPIO_Port, ETH_RST_Pin, GPIO_PIN_SET);
	vTaskDelay(10);
	/* init code for LWIP */
	MX_LWIP_Init();
//	MX_IWDG_Init(IWDG_PRESCALER_4);
	SDlog("Ethernet в работе",0,LSTART);
	/* Create a new TCP connection handle */
	conn = netconn_new(NETCONN_TCP);
	if (conn!= NULL)
	{
	    /* Bind to port 502 (Modbus/TCP) with default IP address */
	    err = netconn_bind(conn, NULL, 502);
	    if (err == ERR_OK)
	    {
	          /* Put the connection into LISTEN state */
	          netconn_listen(conn);
	          SDlog("Ethernet слушает на порту",502,LNETEVENT);
	          for(;;)
	          {
	        	  struct netconn *newconn = NULL;

	        	  ModBus_regs[PLC_SYS_REG] &= ~(1<<ETH_OK);
	        	  /* accept any icoming connection */
	        	  err = netconn_accept(conn,&newconn);
	        	  if(err == ERR_OK)
	        	  {
	        		  struct netbuf *inbuf=NULL;
	        		  uint8_t* buf;
	        		  u16_t buflen;

	        		  ModBus_regs[PLC_SYS_REG] |= (1<<ETH_OK);
	        		  sprintf(str,"Ethernet соединение с %ld.%ld.%ld.%ld установлено",(newconn->pcb.ip->remote_ip.addr)& 0xFF,(newconn->pcb.ip->remote_ip.addr>>8)& 0xFF,(newconn->pcb.ip->remote_ip.addr>>16) & 0xFF,(newconn->pcb.ip->remote_ip.addr>>24) & 0xFF);
	        		  SDlog(str,newconn->type,LNETEVENT);
	        		  netconn_set_recvtimeout ( newconn, 150000 );

	        		  while(netconn_recv(newconn, &inbuf) == ERR_OK)
	        		  {
	        			  netbuf_data(inbuf, (void**)&buf, &buflen);
	        			  SDlog("Ethernet принято байт",buflen,LNETEVENT);
	        			  led_spark(GREEN,10);
	        			  if((buflen > 6) && (*(buf+2) == 0) && (*(buf+3) == 0) && (*(buf+6) == MODBUS_ID))
	        			  {
	        				  Eth_RxTx.buffer = buf + 6;
	        				  Eth_RxTx.counter = buflen - 6;
	        				  MODBUS_SLAVE(&Eth_RxTx,MODBUS_TCP);
	        				  buflen = Eth_RxTx.counter + 6;
	        				  *(buf+5) = Eth_RxTx.counter;
	        				  netconn_write(newconn, buf, (size_t)buflen, NETCONN_NOCOPY);
	        				  SDlog("Ethernet отправлено байт",buflen,LNETEVENT);
	        				  netbuf_delete(inbuf);
	        			  }else
	        			  {
	        				  netbuf_delete(inbuf);
	        				  break;
	        			  }
	        		  }
	        	  }
	        	  /* Close connection and discard connection identifier. */
	        	  sprintf(str,"Ethernet соединение с %ld.%ld.%ld.%ld закрыто",(newconn->pcb.ip->remote_ip.addr)& 0xFF,(newconn->pcb.ip->remote_ip.addr>>8)& 0xFF,(newconn->pcb.ip->remote_ip.addr>>16) & 0xFF,(newconn->pcb.ip->remote_ip.addr>>24) & 0xFF);
	        	  netconn_close(newconn);
	        	  netconn_delete(newconn);
	        	  SDlog(str,0,LNETEVENT);
	          }
	    }else	/* can not bind netconn */
	    {

	    }

	}else 		/* can not create netconn */
	{

	}
	netconn_delete(conn);
	vTaskSuspend( NULL );  /* This task is no longer needed */
  /* USER CODE END StartTaskEth */
}

/* StartTaskADC function */
void CallbackADCimer(xTimerHandle pxTimer)
{
  /* USER CODE BEGIN StartTaskADC */
	uint16_t i_sample,i_channel;
	uint16_t *ptr;
	uint32_t sma;
	static int64_t facu[ADC_CHANELS_NUMBER];
	ptr = &ModBus_regs[ADC_00];

	HAL_ADC_Stop_DMA(&hadc1);
	xSemaphoreTake( myMutexModbusSlaveHandle, portMAX_DELAY );          /* Guard ModBus Slave process */
	for(i_channel=0;i_channel<ADC_CHANELS_NUMBER;++i_channel)
	{
		sma = 0;
		for(i_sample=0;i_sample<ADC_SAMPLE_NUMBER;++i_sample)
		{
			sma += ADC_raw[i_sample][i_channel];						/* Simple moving average */
		}
		sma /= ADC_SAMPLE_NUMBER;
		ptr[i_channel] = sma;
#ifdef EMA_FILTER
		facu[i_channel] += ((sma << 16) - facu[i_channel])/EMA_ORDER;
		ptr[i_channel+ADC_CHANELS_NUMBER] = (uint16_t)(facu[i_channel] >> 16);
#endif /* EMA_FILTER */
	}
	xSemaphoreGive(myMutexModbusSlaveHandle);                              /* Remove the guard ModBus Slave process */
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)ADC_raw, ADC_CHANELS_NUMBER*ADC_SAMPLE_NUMBER);
  /* USER CODE END StartTaskADC */
}

/* StartTaskEEPROM function */
void StartTaskEEPROM(void const * argument)
{
  /* USER CODE BEGIN StartTaskEEPROM */
  uint16_t counter=0;
  /* Infinite loop */
  SDlog("EEPROM в работе",0,LSTART);
  for(;;)
  {
	  vTaskDelay(100);
	  if(HAL_GPIO_ReadPin(GPIOx_KEY, GPIO_Pin_KEY) == GPIO_PIN_SET)
	  {
		  if(++counter == 50)
		  {
			  uint16_t i;
			  led_spark(RED,250);
			  for(i=0;i<sizeof(config_default.data);++i)
			  {
				  config_running->data[i] = config_default.data[i];									/* Copy default config into running config */
			  }
			  config_running->cfg.crc = eeprom.ee_crc_calculate(config_running);						/* Calculate CRC */
			  eeprom.ee_write(ADDRESS_CONFIG_IN_EEPROM,sizeof(config_running->data),config_running->data); /* Save config into eeprom */
			  SDlog("Конфигурация по умолчанию",i,LSYSEVENT);
		  }
		  else
		  {
			  if(counter > 50)
			  {
				  --counter;
			  }
		  }
	  }
	  else
	  {
		  counter=0;
	  }
	  if(config_running->cfg.crc != eeprom.ee_crc_calculate(config_running))						/* Check CRC */
	  {
		  led_spark(RED,250);
		  config_running->cfg.crc = eeprom.ee_crc_calculate(config_running);						/* Calculate CRC */
		  eeprom.ee_write(ADDRESS_CONFIG_IN_EEPROM,sizeof(config_running->data),config_running->data); /* Save config into eeprom */
		  SDlog("Конфигурация обновлена",0,LSYSEVENT);
	  }
  }
  /* USER CODE END StartTaskEEPROM */
}

/* StartTaskSDCard function */
static FATFS fileSystem __attribute__((aligned(4)));
static FIL logFile __attribute__((aligned(4)));
static char logfilename[32];
extern void vApplicationMallocFailedHook(void);
void StartTaskSDCard(void const * argument)
{
  /* USER CODE BEGIN StartTaskSDCard */
	volatile FRESULT res;
	FATFS *fs = &fileSystem;
	log_record_t log_record;
#ifdef CONSOLE
	static buffer8_t SDCard_buffer;
	uint32_t fre_clust;
	uint32_t tot_sect,fre_sect;
	buffer8_t *pSDCard_buffer;
	SDCard_buffer.buffer = (uint8_t*)pvPortMalloc(128);
#endif
	vTaskDelay(200);
	MX_SDIO_SD_Init();

	/* init code for FATFS */
	SD_Path[0] = '0';
	MX_FATFS_Init();
#ifdef CONSOLE
  	  SDCard_buffer.counter = sprintf((char*)SDCard_buffer.buffer,"FATFS Init. Return = %d\n\r",retSD);
	  pSDCard_buffer = &SDCard_buffer;
	  xQueueSend( myQueueUSB_console, ( void * ) &pSDCard_buffer, ( TickType_t ) 0 );
	  vTaskDelay(100);
#endif

	  res=f_mount(&fileSystem, SD_Path, 1);
#ifdef CONSOLE
  	  SDCard_buffer.counter = sprintf((char*)SDCard_buffer.buffer,"Disk mount. Return = %d\n\r",res);
	  pSDCard_buffer = &SDCard_buffer;
	  xQueueSend( myQueueUSB_console, ( void * ) &pSDCard_buffer, ( TickType_t ) 0 );
	  vTaskDelay(150);
#endif
  if(res == FR_OK)
  {
	  ModBus_regs[PLC_SYS_REG] |= (1<<SD_MOUNT_OK);
#ifdef CONSOLE
	  res = f_getfree(SD_Path, &fre_clust, &fs);
	  SDCard_buffer.counter = sprintf((char*)SDCard_buffer.buffer,"Get Free space. Return = %d\n\r",res);
	  pSDCard_buffer = &SDCard_buffer;
	  xQueueSend( myQueueUSB_console, ( void * ) &pSDCard_buffer, ( TickType_t ) 0 );
	  tot_sect = (fileSystem.n_fatent - 2) * fileSystem.csize * _MAX_SS;
	  fre_sect = fre_clust * fileSystem.csize * _MAX_SS;
	  SDCard_buffer.counter = sprintf((char*)SDCard_buffer.buffer,"%lu B total drive space.\n\r%lu B available.\n\r",tot_sect ,fre_sect);
	  pSDCard_buffer = &SDCard_buffer;
	  xQueueSend( myQueueUSB_console, ( void * ) &pSDCard_buffer, ( TickType_t ) 0 );
	  vTaskDelay(200);
#endif
	  /* Infinity Loop Of SD Logger */
	  for(;;)
	  {
		  DWORD date_time,fre_clust,tot_bytes,free_bytes;

//		  vTaskDelay(100);

		  /* Check SDCard Used Space */
		  res = f_getfree(SD_Path, &fre_clust, &fs);
#ifdef CONSOLE
  	  SDCard_buffer.counter = sprintf((char*)SDCard_buffer.buffer,"Get Free space. Return = %d\n\r",res);
	  pSDCard_buffer = &SDCard_buffer;
	  xQueueSend( myQueueUSB_console, ( void * ) &pSDCard_buffer, ( TickType_t ) 0 );
#endif
		  tot_bytes = (fileSystem.n_fatent - 2) * fileSystem.csize * _MAX_SS;
		  tot_bytes = (tot_bytes > 0xFFFFFFFF)? tot_bytes:0xFFFFFFFF;
		  free_bytes = fre_clust * fileSystem.csize * _MAX_SS;
		  if(free_bytes < (tot_bytes/8))
		  {
			  /* Find & Kill Oldest File */
		  }


		  if(xQueueReceive(myQueueSDCardHandle,&log_record,portMAX_DELAY) == pdTRUE)
		  {
			  date_time = get_fattime();
			  sprintf(	logfilename,							/* Create The File Name From Date */
					  	  "RB%02d%02d%02d.csv",
					  	  (int)((date_time >> 25) + 1980 - 2000),
					  	  (int)((date_time >> 21) & 0x0f),
					  	  (int)((date_time >> 16) & 0x1f));
//			  taskENTER_CRITICAL();
			  res = f_open(&logFile, logfilename, FA_WRITE | FA_OPEN_ALWAYS);		/* And Open It */
//	  	  	  taskEXIT_CRITICAL();
#ifdef CONSOLE
  	  SDCard_buffer.counter = sprintf((char*)SDCard_buffer.buffer,"Open File. Return = %d\n\r",res);
	  pSDCard_buffer = &SDCard_buffer;
	  xQueueSend( myQueueUSB_console, ( void * ) &pSDCard_buffer, ( TickType_t ) 0 );
#endif
			  taskYIELD();
			  if(res != FR_OK)
			  {
				  break;
			  }
			  taskENTER_CRITICAL();
			  res = f_lseek(&logFile, f_size(&logFile));							/* Go To End Of The File */
			  taskEXIT_CRITICAL();
			  taskYIELD();
			  if(res != FR_OK)
			  {
				  break;
			  }
#ifdef CONSOLE
	SDCard_buffer.counter = sprintf((char*)SDCard_buffer.buffer,"File Seek. Res = %d\n\r",res);
	pSDCard_buffer = &SDCard_buffer;
	xQueueSend( myQueueUSB_console, ( void * ) &pSDCard_buffer, ( TickType_t ) 0 );
#endif
			  do
			  {

				  taskENTER_CRITICAL();													/* Write Message To The File */
				  res = f_printf(&logFile,
					  	  	  "%02d:%02d:%02d,%d,%s,%d,\n",
					  	  	  log_record.hours_minutes >> 8,
					  	  	  log_record.hours_minutes & 0xFF,
					  	  	  log_record.seconds,
					  	  	  log_record.level,
					  	  	  log_record.message,
					  	  	  log_record.value);
				  taskEXIT_CRITICAL();
#ifdef CONSOLE
	SDCard_buffer.counter = sprintf((char*)SDCard_buffer.buffer,"File Write. Res = %d\n\r",res);
	pSDCard_buffer = &SDCard_buffer;
	xQueueSend( myQueueUSB_console, ( void * ) &pSDCard_buffer, ( TickType_t ) 0 );
#endif
				  taskYIELD();
			  }while((xQueueReceive(myQueueSDCardHandle,&log_record,0) == pdTRUE) && (res != EOF));
			  taskENTER_CRITICAL();
			  res = f_close(&logFile);											/* Close The File */
			  taskEXIT_CRITICAL();
#ifdef CONSOLE
	SDCard_buffer.counter = sprintf((char*)SDCard_buffer.buffer,"File Close. Res = %d\n\r",res);
	pSDCard_buffer = &SDCard_buffer;
	xQueueSend( myQueueUSB_console, ( void * ) &pSDCard_buffer, ( TickType_t ) 0 );
#endif
			  taskYIELD();
			  if(res != FR_OK)
			  {
				  break;
			  }
		  }
	  }
  /* USER CODE END StartTaskSDCard */
  }
  ModBus_regs[PLC_SYS_REG] &= ~(1<<SD_MOUNT_OK);
  f_mount(NULL, SD_Path, 1);		/* Unmount File System */
  vTaskSuspend( NULL );  			/* This task is no longer needed */
}

/* StartTaskPLC function */
void StartTaskPLC(void const * argument)
{
  /* USER CODE BEGIN StartTaskPLC */
	  static signal_t signal;
	  /* Main PLC Queue */
	  xPLC_Queue = xQueueCreate( QUEUE_PLC_LENGTH, ITEM_SIZE_QUEUE_PLC );
	  /* Make Software Timer for PLC Process */
	  xPLC_Timer = xTimerCreate("PLC_Timer",PLC_TIMER_QUANTUM,pdTRUE,( void * ) PLC_TIMER_ID,PLC_Signals_Generate);

	  PLC_init();
	  vTaskDelay(100);
	  xTimerStart( xPLC_Timer, 0);
	  SDlog("PLC в работе",0,LSTART);
	  for(;;)
	  {
	    /* Wait signals */
	    if(xQueueReceive( xPLC_Queue, &(signal), 10) == pdTRUE)
	    {
	        /* Signals process */
	        PLC_process_sequential(signal);
	        PLC_Signals_Generate(xPLC_Timer);
	    }
	    else
	    {

	    }
	  }
  /* USER CODE END StartTaskPLC */
}

/* StartTaskDI function */
void CallbackDIOTimer(xTimerHandle pxTimer)
{
  /* USER CODE BEGIN StartTaskDI */
	uint16_t i;
	xSemaphoreTake( myMutexModbusSlaveHandle, portMAX_DELAY );          /* Guard ModBus Slave process */
	for(i=0;i<(sizeof(di_prepare)/sizeof(di_prepare_t));++i)
	{
		switch(di_prepare[i].state)
		{
		case DI_OFF:
			if(!(READ_BIT(dinputs[i].port->IDR,dinputs[i].pin)))
			{
				if((di_prepare[i].time += DIO_TIMER_QUANTUM) >= di_prepare[i].timer)
				{
					di_prepare[i].state = DI_ON;
					ModBus_regs[ONBOARD_DI] |= (1<<i);
				}
			}
			else
			{
				di_prepare[i].time = 0;
			}
			break;
		case DI_ON:
			if(!(READ_BIT(dinputs[i].port->IDR,dinputs[i].pin)))
			{
				di_prepare[i].time = di_prepare[i].timer;
			}
			else
			{
				if((di_prepare[i].time -= DIO_TIMER_QUANTUM) == 0)
				{
					di_prepare[i].state = DI_OFF;
					ModBus_regs[ONBOARD_DI] &= ~(1<<i);
				}
			}
			break;
		default:
			di_prepare[i].state = DI_OFF;
			ModBus_regs[ONBOARD_DI] &= ~(1<<i);
			break;
		}
	}
//	++ModBus_regs[ONBOARD_DO];
	for(i=0;i<8;++i)
	{
		if(ModBus_regs[ONBOARD_DO] & (1<<i))
		{
			SET_BIT(doutputs[i].port->ODR,doutputs[i].pin);
		}
		else
		{
			CLEAR_BIT(doutputs[i].port->ODR,doutputs[i].pin);
		}
	}
	xSemaphoreGive(myMutexModbusSlaveHandle);                            /* Remove the guard ModBus Slave process */
  /* USER CODE END StartTaskDI */
}

#define SECONDS_IN_MONTH	(60*60*24*30)
/* RTC task */
void CallbackRTCTimer(xTimerHandle pxTimer)
{
	  static RTC_TimeTypeDef sTime;
	  static RTC_DateTypeDef sDate;
	  static uint8_t last_second=61;
	  uint8_t set_time = pdFALSE;
	  uint8_t set_date = pdFALSE;

	  uint32_t calibration_value = hrtc.Instance->CALIBR;

	  /* Set calibration */
	  if(ModBus_regs[SET_CALIBRATION])
	  {
		  ModBus_regs[SET_CALIBRATION+1] = (int16_t)calibration_value;
		  int16_t tmp = ModBus_regs[SET_CALIBRATION];
		  ModBus_regs[SET_CAL] = ModBus_regs[SET_CALIBRATION];
		  uint32_t calibration_sign = RTC_CALIBSIGN_POSITIVE;
		  if(calibration_value & RTC_CALIBSIGN_NEGATIVE)		/* It sets a negative calibration */
		  {
			  calibration_value &= ~RTC_CALIBSIGN_NEGATIVE;
			  tmp -= (calibration_value << 1) * SECONDS_IN_MONTH / 1048576;
		  }
		  else													/* It sets a positive calibration */
		  {
			  tmp += (calibration_value << 2) * SECONDS_IN_MONTH / 1048576;
		  }
		  ModBus_regs[CAL_FROM] = (uint16_t)tmp;
		  if(tmp >= 0) 	/* positive calibration */
		  {
			  calibration_value = ((1048576 * tmp) / SECONDS_IN_MONTH) >> 2;
		  }
		  else			/* negative calibration */
		  {
			  calibration_value = ((1048576 * (0 - tmp)) / SECONDS_IN_MONTH) >> 1;
			  if(calibration_value) calibration_sign = RTC_CALIBSIGN_NEGATIVE;
		  }
		  ModBus_regs[CAL_TO] = (uint16_t)(calibration_sign | (0x001F & calibration_value));
		  HAL_RTCEx_SetCoarseCalib(&hrtc, calibration_sign, (0x001F & calibration_value));
		  ModBus_regs[SET_CALIBRATION] = 0;
		  SDlog("Калибовка RTC",ModBus_regs[CAL_TO],LSYSEVENT);
	  }
	  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	  if(last_second != sTime.Seconds)					/* Every second calculate operating time */
	  {
		  calculate_operating_time();
		  last_second = sTime.Seconds;
	  }
	  HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	  /* Change Time */
	  if(ModBus_regs[SET_HOUR])
	  {
		  sTime.Hours = (0x001f & ModBus_regs[SET_HOUR]);
		  ModBus_regs[SET_HOUR] = 0;
		  set_time = pdTRUE;
		  SDlog("Установлены часы",sTime.Hours,LSYSEVENT);
	  }
	  if(ModBus_regs[SET_MINUTE])
	  {
		  sTime.Minutes = (0x003f & ModBus_regs[SET_MINUTE]);
		  ModBus_regs[SET_MINUTE] = 0;
		  set_time = pdTRUE;
		  SDlog("Установлены минуты",sTime.Minutes,LSYSEVENT);
	  }
	  if(ModBus_regs[SET_SECOND])
	  {
		  sTime.Seconds = (0x003f & ModBus_regs[SET_SECOND]);
		  ModBus_regs[SET_SECOND] = 0;
		  set_time = pdTRUE;
		  SDlog("Установлены секунды",sTime.Seconds,LSYSEVENT);
	  }
	  if(set_time == pdTRUE)
	  {
		  HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
		  set_time = pdFALSE;
	  }
	  /* Change Date */
	  if(ModBus_regs[SET_YEAR])
	  {
		  sDate.Year = ModBus_regs[SET_YEAR];
		  ModBus_regs[SET_YEAR] = 0;
		  set_date = pdTRUE;
		  SDlog("Установлен год",sDate.Year,LSYSEVENT);
	  }
	  if(ModBus_regs[SET_MONTH])
	  {
		  sDate.Month = ModBus_regs[SET_MONTH];
		  ModBus_regs[SET_MONTH] = 0;
		  set_date = pdTRUE;
		  SDlog("Установлен месяц",sDate.Month,LSYSEVENT);
	  }
	  if(ModBus_regs[SET_DAY])
	  {
		  sDate.Date = ModBus_regs[SET_DAY];
		  ModBus_regs[SET_DAY] = 0;
		  set_date = pdTRUE;
		  SDlog("Установлен день",sDate.Date,LSYSEVENT);
	  }
	  if(ModBus_regs[SET_WEEKDAY])
	  {
		  sDate.WeekDay = ModBus_regs[SET_WEEKDAY];
		  ModBus_regs[SET_WEEKDAY] = 0;
		  set_date = pdTRUE;
		  SDlog("Установлен день недели",sDate.WeekDay,LSYSEVENT);
	  }
	  if(set_date == pdTRUE)
	  {
		  HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
		  set_date = pdFALSE;
	  }

	  ModBus_regs[DAY_MONTH] = (sDate.Date << 8) | (sDate.Month & 0xFF);
	  ModBus_regs[YEAR] = sDate.Year;
	  ModBus_regs[HOURS_MINUTES] = (sTime.Hours << 8) | (sTime.Minutes & 0xFF);
	  ModBus_regs[SECONDS] = sTime.Seconds;

}
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
