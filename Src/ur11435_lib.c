/*
 * ur11435.c
 *
 *  Created on: Aug 26, 2016
 *      Author: andrew
 */

#include "cmsis_os.h"
#include "ur11435_lib.h"
#include "stm32f2xx_hal.h"
#include "tim.h"
#include "modbus_slave.h"

/* LED Control */
typedef enum {OFF=0,ON,SPARK,BLINK} led_mode_t;

buffer8_t com3_RxTx;
uint8_t com3_RxByte;
uint8_t com3_Mode;
const dio_t dinputs[] = {
		{GPIOE,GPIO_PIN_7},
		{GPIOE,GPIO_PIN_8},
		{GPIOE,GPIO_PIN_9},
		{GPIOE,GPIO_PIN_10},
		{GPIOE,GPIO_PIN_11},
		{GPIOE,GPIO_PIN_12},
		{GPIOE,GPIO_PIN_13},
		{GPIOE,GPIO_PIN_14},
		{GPIOD,GPIO_PIN_4},
		{GPIOD,GPIO_PIN_3},
		{GPIOD,GPIO_PIN_1},
		{GPIOD,GPIO_PIN_0},
		{GPIOA,GPIO_PIN_15},
		{GPIOA,GPIO_PIN_9},
		{GPIOC,GPIO_PIN_7},
		{GPIOC,GPIO_PIN_6},

};
const dio_t doutputs[] = {
		{GPIOD,GPIO_PIN_15},
		{GPIOD,GPIO_PIN_14},
		{GPIOD,GPIO_PIN_13},
		{GPIOD,GPIO_PIN_12},
		{GPIOD,GPIO_PIN_11},
		{GPIOD,GPIO_PIN_10},
		{GPIOD,GPIO_PIN_9},
		{GPIOD,GPIO_PIN_8},
};
static struct led_t
{
	color_t			color;
	led_mode_t		mode;
}led;

void led_on(color_t color)
{
	led_off();
	switch(led.color = color)
	{
	case RED:
		HAL_GPIO_WritePin(GPIOE, PE2_LED_RED_Pin, GPIO_PIN_SET);
		break;
	case GREEN:
		HAL_GPIO_WritePin(GPIOE, PE3_LED_GREEN_Pin, GPIO_PIN_SET);
		break;
	default:
		return;
	}
	led.mode = ON;
}
void led_off(void)
{
	led.mode = OFF;
	HAL_GPIO_WritePin(GPIOE, PE2_LED_RED_Pin|PE3_LED_GREEN_Pin, GPIO_PIN_RESET);
}
void led_spark(color_t color,uint16_t milisec)
{
	led_on(color);
	led.mode = SPARK;
	xTimerChangePeriod (myTimerLEDHandle, milisec,0);
}
void led_blink(color_t color,uint16_t milisec)
{
	led_on(color);
	led.mode = BLINK;
	xTimerChangePeriod (myTimerLEDHandle, milisec,0);
}
/* CallbackLED function */
void CallbackLED(xTimerHandle pxTimer)
{
  /* USER CODE BEGIN CallbackLED */
  if(led.mode == BLINK)
  {
	switch(led.color)
	{
	case RED:
		HAL_GPIO_TogglePin(GPIOE, PE2_LED_RED_Pin);
		return;
	case GREEN:
		HAL_GPIO_TogglePin(GPIOE, PE3_LED_GREEN_Pin);
		return;
	default:
		break;
	}
  }
  led_off();
  xTimerStop(myTimerLEDHandle,0);
  /* USER CODE END CallbackLED */
}
static void stop_break(TIM_HandleTypeDef *htim)
{
	__HAL_TIM_SetCounter(htim, 0);
    HAL_TIM_Base_Stop_IT(htim);
}
static void start_break(TIM_HandleTypeDef *htim)                                                   /* Start 3.5 time break */
{
    __HAL_TIM_SetCounter(htim, 0);
    __HAL_TIM_CLEAR_IT(htim, TIM_IT_UPDATE);
    HAL_TIM_Base_Start_IT(htim);
}
/* USB BreakTime ModBus timer */
void usb_stop_break(void)

{
	stop_break(&htim7);
}
void usb_start_break(void)                                                   /* Start 3.5 time break */
{
	start_break(&htim7);
}
/* COM1 BreakTime ModBus timer */
void com1_stop_break(void)                                                    /* Stop 3.5 time break */
{
	stop_break(&htim6);
}
void com1_start_break(void)                                                   /* Start 3.5 time break */
{
	start_break(&htim6);
}
void com3_stop_break(void)                                                    /* Stop 3.5 time break */
{
	stop_break(&htim9);
}
void com3_start_break(void)                                                   /* Start 3.5 time break */
{
	start_break(&htim9);
}
