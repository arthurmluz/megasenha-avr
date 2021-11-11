#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"

#define ROWS 3
#define COLUMNS 9

char keys[ROWS][COLUMNS] = {"123456789",
                            "         ",
                            "  < v >  "};

char column_pins[COLUMNS]                       = {PC6,    PD0,    PD1,    PD2,    PD3,    PD4,    PB6,    PB7,    PD5};
volatile uint8_t *column_read_pins[COLUMNS]     = {&PINC,  &PIND,  &PIND,  &PIND,  &PIND,  &PIND,  &PINB,  &PINB,  &PIND};
volatile uint8_t *column_ports[COLUMNS]         = {&PORTC, &PORTD, &PORTD, &PORTD, &PORTD, &PORTD, &PORTB, &PORTB, &PORTD};
volatile uint8_t *column_ddrs[COLUMNS]          = {&DDRC,  &DDRD,  &DDRD,  &DDRD,  &DDRD,  &DDRD,  &DDRB,  &DDRB,  &DDRD};

char row_pins[ROWS]                  = {PD6,    PD7,    PB0};
volatile uint8_t *row_ports[ROWS]    = {&PORTD, &PORTD, &PORTB};
volatile uint8_t *row_ddrs[COLUMNS]  = {&DDRD, &DDRD, &DDRB};

void keypad_init() {
    for(int i = 0; i < ROWS; i++) {
        volatile uint8_t *ddr = row_ddrs[i];
        volatile uint8_t *port = row_ports[i];
        char pin = row_pins[i];

        *ddr |= (1 << pin);
        *port &= ~(1 << pin);
    }

    for(int i = 0; i < COLUMNS; i++) {
        volatile uint8_t *ddr = column_ddrs[i];
        volatile uint8_t *port = column_ports[i];
        char pin = column_pins[i];

        *ddr &= ~(1 << pin);
        *port &= ~(1 << pin);
    }
} 

char keypad_poll() {
    for(int row = 0; row < ROWS; row++) {
        volatile uint8_t *row_port = row_ports[row];
        char row_pin = row_pins[row];

        *row_port |= (1 << row_pin);
        _delay_ms(5);

        for(int col = 0; col < COLUMNS; col++) {
            volatile uint8_t *col_read_pin = column_read_pins[col];
            char col_pin = column_pins[col];
            
            if((*col_read_pin) & (1 << col_pin)) {  
                *row_port &= ~(1 << row_pin);

                return keys[row][col];
            }
        }

        *row_port &= ~(1 << row_pin);
    }

    return 0;
}