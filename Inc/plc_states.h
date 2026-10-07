#ifndef PLC_STATES_H_INCLUDED
#define PLC_STATES_H_INCLUDED

typedef enum { STATE_NO_NEW = 0,
                STATE_ANY,
                STATE_INIT,STATE_OFF,STATE_READY,STATE_ON,STATE_ERROR,STATE_WAIT,
                STATE_LAST,STATE_ACCELERATION,STATE_ACCELERATION_READY,STATE_HOISTDRIVEON

}state_t;
// Тип указателя на функцию
typedef void (*fuction_pointer_t)(void);
typedef struct{
    state_t                 current_state;          // Текущее состояние
    signal_t                received_signal;        // Принятое событие
    fuction_pointer_t       execute_function;       // Выполнить функцию
    state_t                 next_state;             // Перейти в состояние
}state_table_t;
typedef struct{
    const state_table_t    *state_table;
    uint16_t                size_of_table;
    uint16_t                current_state;
}state_machine_t;
extern state_machine_t state_machines_table[];
extern const uint32_t sizeof_state_machines_table;

/******************** Functions in plc ******************************/
void Test1_func(void);
void null_function(void);					/*все функции, используемые в программе, должны быть объявлены тут*/

#endif /* PLC_STATES_H_INCLUDED */
