#include <avr/io.h>

#define DATA_DDR DDRC
#define DATA_PORT PORTC
#define DATA_PIN PC5

#define CLOCK_DDR DDRC
#define CLOCK_PORT PORTC
#define CLOCK_PIN PC4

void shift_reg_init();
void shift_reg_write(uint8_t data);