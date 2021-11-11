#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"

#define ROWS 3
#define COLUMNS 9

char keys[ROWS][COLUMNS] = {"123456789", "\0\0\0\0\0\0\0\0\0", {'\0', '\0', '<', '\0', 'v', '\0', '>', '\0', '\0'}};

char pins_in[COLUMNS] = {PC6, PD0, PD1, PD2, PD3, PD4, PB6, PB7, PD5};
volatile uint8_t *p_in[COLUMNS] = {&PINC, &PIND, &PIND, &PIND, &PIND, &PINB, &PINB, &PIND};
volatile uint8_t *ports_in[COLUMNS] = {&PORTC, &PORTD, &PORTD, &PORTD, &PORTD, &PORTB, &PORTB, &PORTD};

char pins_out[ROWS] = {PD6, PD7, PB0};
volatile uint8_t *ports_out[ROWS] = {&PORTD, &PORTD, &PORTB};

void keypad_init() {
    for(int i = 0; i < ROWS; i++) {
        volatile uint8_t *port_out = ports_out[i];
        char pin_out = pins_out[i];

        *port_out |= (1 << pin_out);
    }

    for(int i = 0; i < COLUMNS; i++) {
        volatile uint8_t *port_in = ports_in[i];
        char pin_in = pins_in[i];

        *port_in &= ~(1 << pin_in);
    }
} 

void print(char *s) {
    while(*s) {
        uart_tx(*s++);
    }
}

char keypad_poll() {
    for(int out = 0; out < ROWS; out++) {
        volatile uint8_t *port_out = ports_out[out];
        char pin_out = pins_out[out];

        *port_out |= (1 << pin_out);
        _delay_ms(20);

        for(int in = 0; in < COLUMNS; in++) {
            volatile uint8_t *port_in = p_in[in];
            char pin_in = pins_in[in];

            if((*port_in) & (1 << pin_in)) {  
                *port_out &= ~(1 << pin_out);
                print("hello!\n");
                return keys[out][in];
            }
        }

        *port_out &= ~(1 << pin_out);
    }

    return 0;
}