#ifndef DCON_H_INCLUDED
#define DCON_H_INCLUDED

#define DCON_BAUDRATE               (57600UL)
#define DCON_REQUEST_LENGTH         (64)
// Uncomment if enable CRC
#define DCON_CHECK_CRC
#define DCON_TIMEOUT                (10)
#define DCON_CR                     (0x0D)
#define DCON_TE_ON 		HAL_GPIO_WritePin(USART2_TE_GPIO_Port, USART2_TE_Pin, GPIO_PIN_SET)
#define DCON_TE_OFF 	HAL_GPIO_WritePin(USART2_TE_GPIO_Port, USART2_TE_Pin, GPIO_PIN_RESET)

#define DCON_PORT                   USART2
#define hDCON_PORT                  huart2

extern uint8_t *DCON_request;
extern volatile uint16_t    DCON_net_time;

void dcon_init(void);
uint16_t dcon_modules_number(void);
void dcon_next_inputs_module(void);
void dcon_dio_read_inputs(void);
void dcon_dio_write_outputs(void);
uint8_t dcon_write_check(uint16_t);
void dcon_host_OK_request(void);
void str_copy(uint8_t *to,uint8_t *from);
void str_cat(uint8_t *to,uint8_t *from);
void dcon_set_register(uint8_t mode);
void dcon_reset_register(uint8_t mode);
void dcon_write_inputs_to_modbus_reg(uint8_t *buffer);
uint16_t dcon_current_vodule(char);
#endif /* DCON_H_INCLUDED */
