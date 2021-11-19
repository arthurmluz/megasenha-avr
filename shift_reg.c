#include <util/delay.h>
#include "./shift_reg.h"

void shift_reg_init() {
    CLOCK_DDR |= (1 << CLOCK_PIN);
    DATA_DDR |= (1 << DATA_PIN);
}

void shift_reg_write(uint8_t data) {
    for(int i = 0; i < 8; i++) {
        if(data & (1 << i)) {
            DATA_PORT |= (1 << DATA_PIN);
        } else {
            DATA_PORT &= ~(1 << DATA_PIN);
        }

        CLOCK_PORT |= (1 << CLOCK_PIN);
        _delay_ms(2);
        CLOCK_PORT &= ~(1 << CLOCK_PIN);
        _delay_ms(2);
    }
}