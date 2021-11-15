
#include <stdio.h>
#include <avr/io.h>
#include <stdlib.h>
#include <ctype.h>
#include <util/delay.h>
#include "nokia5110.h"
#include "keypad.h"
#include "uart.h"

#define RIGHT '>'
#define LEFT '<'
#define SELECT 'v' 
#define TRY 'T'

uint8_t glyph[] = { 0b00111000,0b01101000,0b11001000,0b01101000,0b00111000 };

uint8_t glyph2[] = {0b00111000,0b01111000,0b11111000,0b01111000,0b00111000 };

typedef struct sel{
    int x, y;
} select;

// global variables YES, global. I know what i'm doing :)
int password[4];
int guess[4];
select cursor = {.x = 20, .y = 0};


void initializer(){
    nokia_lcd_init();
    nokia_lcd_clear();
    nokia_lcd_custom(1,glyph);
    nokia_lcd_custom(2,glyph2);

    keypad_init();
    uart_init(57600, 1);
}

void escreveVetor(int vetor[]){
    char buffer[100];
    nokia_lcd_set_cursor(20, 12);
    snprintf(buffer, sizeof(buffer), "%d %d %d %d", vetor[0], vetor[1], vetor[2], vetor[3]);
    nokia_lcd_write_string(buffer, 1);
}

void makePassword(){
    password[0] = 1 + rand() % 9;
    password[1] = 1 + rand() % 9;
    password[2] = 1 + rand() % 9;
    password[3] = 1 + rand() % 9;
}

void redraw(select cursor, int vetor[], int bool ){
    nokia_lcd_clear();
    escreveVetor(vetor);
    nokia_lcd_set_cursor(cursor.x, cursor.y);
    if(bool)
        nokia_lcd_write_string("\002", 1);
    else
        nokia_lcd_write_string("\001", 1);

    nokia_lcd_render();
}

int moveCursor(char value, int selected){

    switch(value){
        case RIGHT:
            if( selected == 0) {
                //print("right");
                if(cursor.x >= 56)
                   cursor.x = 20;
                else cursor.x += 12;
                redraw(cursor, guess, 0);
                _delay_ms(100);
            }
            break;

        case LEFT:
            if( selected == 0 ){
         //       print("left");
                if(cursor.x <= 20)
                   cursor.x = 56;
                else cursor.x -= 12;
                redraw(cursor, guess, 0);
                _delay_ms(100);
            }
            break;

        case SELECT:
            selected = 1 - selected;
            redraw(cursor, guess, selected);
            _delay_ms(100);

    }

    return selected;
}

int 

int main(void){
    initializer();
    makePassword();

    guess[0] = 1; guess[1] = 1; guess[2] = 1; guess[3] = 1;

    redraw(cursor, guess, 0 ); // initialize the screen
    _delay_ms(10);

    int selected = 0;
    while(1) {
        
        char c = keypad_poll();

        selected = moveCursor(c, selected);
        
        c = keypad_poll();

        if( selected ){
            int arrayPos = (cursor.x-20) / 12; 
            if( isdigit(c) ){
                guess[arrayPos] = (int) c - '0';
                redraw(cursor, guess, selected);
            }
        }


    }
}