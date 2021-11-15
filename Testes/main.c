#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "adc.h"
#include "print.h"
#include "nokia5110.h"
#include <stdlib.h>


#define RIGHT 1
#define LEFT 0
#define SELECT 1

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
    sei();
    nokia_lcd_init();
    adc_init();
    nokia_lcd_custom(1,glyph);
    nokia_lcd_custom(2,glyph2);
}

void escreveVetor(int vetor[]){
    char buffer[100];
    nokia_lcd_set_cursor(20, 12);
    snprintf(buffer, sizeof(buffer), "%d %d %d %d", vetor[0], vetor[1], vetor[2], vetor[3]);
    nokia_lcd_write_string(buffer, 1);
}

void clear(select cursor, int vetor[], int bool ){
    nokia_lcd_clear();
    escreveVetor(vetor);
    nokia_lcd_set_cursor(cursor.x, cursor.y);
    if(bool)
        nokia_lcd_write_string("\002", 1);
    else
        nokia_lcd_write_string("\001", 1);

    nokia_lcd_render();
}

void makePassword(){
    password[0] = 1 + rand() % 9;
    password[1] = 1 + rand() % 9;
    password[2] = 1 + rand() % 9;
    password[3] = 1 + rand() % 9;
}

int moveCursor(float direction, float selecter, int selected){
    if(direction == RIGHT && selected == 0){
        print("right");
        if(cursor.x >= 56)
           cursor.x = 20;
        else cursor.x += 12;
        clear(cursor, guess, 0);
        _delay_ms(100);
    }
    if(direction == LEFT && selected == 0){
        print("left");
        if(cursor.x <= 20)
           cursor.x = 56;
        else cursor.x -= 12;
        clear(cursor, guess, 0);
        _delay_ms(100);
    }
    if(selecter == SELECT){
        selected = 1 - selected;
        clear(cursor, guess, selected);
        _delay_ms(100);
    }

    return selected;
}

int main(){
    initializer(); 
    makePassword();

    guess[0] = 0; 

    clear(cursor, guess, 0 ); // initialize the screen
    _delay_ms(10);

    int selected = 0;
    while(1){
        adc_set_channel(0);
        _delay_ms(100);
        float direction = adc_read() / 1021.0;
        adc_set_channel(1);
        _delay_ms(100);
        float value = adc_read() / 1021.0;

        selected = moveCursor(direction, value, selected); // move selector
        if( selected ){
            int arrayPos = (cursor.x-20) / 12;
            if( value == 0 ){
                guess[arrayPos] += 1;
                if (guess[arrayPos] >= 10)
                    guess[arrayPos] = 0;
                clear(cursor, guess, selected);
            }
        }

        print("\ndirection: ");
        printfloat(direction);
        print(" value: ");
        printfloat(value);

    }
    

    return 0;
}