#include <avr/io.h>
#include <util/delay.h>
#include "./ws2812.h"

#define PORT PORTC
#define P PC5
#define DDR DDRC

void ws2812_init() {
    DDR |= (1 << P);
}

void ws2812_reset() {
    PORT &= ~(1 << P);
    _delay_us(51);
}

void ws2812_send_byte(uint8_t byte) {
    for(int i = 0; i < 8; i++) {
        char bit = byte & (1 << i);

        if(bit) {
            PORT |= (1 << P);
            _delay_us(0.7);
            PORT &= ~(1 << P);
            _delay_us(0.6);
        } else {
            PORT |= (1 << P);
            _delay_us(0.35);
            PORT &= ~(1 << P);
            _delay_us(0.8);
        }
    }
}

void ws2812_display(rgb_s *colors, uint32_t len) {
    ws2812_reset();
    for(int i = 0; i < len; i++) {
        ws2812_send_byte(colors[i].g);
        ws2812_send_byte(colors[i].r);
        ws2812_send_byte(colors[i].b);
    }
    ws2812_reset();
}

