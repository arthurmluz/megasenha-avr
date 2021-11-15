/* Nokia 5110 LCD AVR Library example
 *
 * Copyright (C) 2015 Sergey Denisov.
 * Written by Sergey Denisov aka LittleBuster (DenisovS21@gmail.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licence
 * as published by the Free Software Foundation; either version 3
 * of the Licence, or (at your option) any later version.
 *
 * Original library written by SkewPL, http://skew.tk
 * Custom char code by Marcelo Cohen - 2021
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "nokia5110.h"
#include "keypad.h"
#include "uart.h"
#include "ws2812.h"

rgb_s colors[3] = {{.r = 255, .g = 0, .b = 0}, {.r = 0, .g = 255, .b = 0}, {.r = 0, .g = 0, .b = 255}};

int main(void)
{
    ws2812_init();
    while(1){
        ws2812_display(colors, 3);
    }

    nokia_lcd_init();
    nokia_lcd_clear();
    nokia_lcd_set_cursor(0,12);

    // nokia_lcd_drawcircle(20,20,20);
    // while(1);
    //
    int password[4];
    password[0] = 1 + rand() % 9;
    password[1] = 1 + rand() % 9;
    password[2] = 1 + rand() % 9;
    password[3] = 1 + rand() % 9;

    keypad_init();

    nokia_lcd_set_cursor(0, 0);
    nokia_lcd_write_char('A', 1);
    nokia_lcd_render();



    uart_init(57600, 1);
    while(1) {
        
        

    }
}