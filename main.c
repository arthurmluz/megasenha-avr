
#include <stdio.h>
#include <avr/io.h>
#include <stdlib.h>
#include <ctype.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "nokia5110.h"
#include "keypad.h"
#include "print.h"

#define RIGHT '>'
#define LEFT '<'
#define SELECT 'v' 
#define TRY 'T'


uint8_t glyph[] = { 0b00111000,0b01101000,0b11001000,0b01101000,0b00111000 };

uint8_t glyph2[] = {0b00111000,0b01111000,0b11111000,0b01111000,0b00111000 };

uint8_t glyph3[] = {0b00011000,0b00100100,0b01001000,0b00100100,0b00011000 };

typedef struct sel{
    int x, y;
} select;

// global variables YES, global. I know what i'm doing :)
int password[4];
int guess[4];
select cursor = {.x = 20, .y = 0};
int tentativas = 9;
int secs_remaining = 30;

void initializer(){
    nokia_lcd_init();
    nokia_lcd_clear();
    nokia_lcd_custom(1,glyph);
    nokia_lcd_custom(2,glyph2);
    nokia_lcd_custom(3,glyph3);


    keypad_init();
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

    // Cursor
    if(bool)
        nokia_lcd_write_string("\002", 1);
    else
        nokia_lcd_write_string("\001", 1);

    // Tempo
    char secs_remaining_str[3];
    itoa(secs_remaining, secs_remaining_str, 10);

    nokia_lcd_set_cursor(4, 40);
    nokia_lcd_write_string(secs_remaining_str, 1);

    // Tentativas
    nokia_lcd_set_cursor(60, 34);
    nokia_lcd_write_string("\003", 2);

    nokia_lcd_set_cursor(72, 40);
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "%d", tentativas+1);
    nokia_lcd_write_string(buffer, 1);



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

int tryToGuess(int validadas[2]){
    // validadas 0 = Posições Corretas
    // validadas 1 = Posições incorretas
    validadas[0] = 0;
    validadas[1] = 0;
    for(int i = 0; i < 4; i++ ){
        if( guess[i] == password[i] ) validadas[0]++;
        for(int j = i+1; j < 4; j++)
            if( guess[j] == password[i] ) validadas[1]++;
    }
    if( validadas[0] == 4)
        return 1;

    return 0;
}

ISR(TIMER1_COMPA_vect) {
    secs_remaining -= 1;

    if(secs_remaining <= 0) {
        tentativas = 0;
    }
}

void setup_timer() {
    TCCR1B = (1 << CS12) | (1 << CS10) | (1 << WGM12);

    // Set the timer's compare match to 15624 because the uc's frequency
    // is 16,000,000 Hz and we're using a prescaler of 1024. So the counter
    // is incremented 15625 times a second. 15624 = 15625 - 1, I'm not sure
    // why we need to subtract 1, but that's what the datasheet says.
    OCR1AH = 15624 >> 8;
    OCR1AL = 15624 & 0x00ff;

    TIMSK1 = (1 << OCIE1A);
}

int main(void){
    setup_timer();
    sei();

    initializer();
    makePassword();
    printint(password[0]);
    printint(password[1]);
    printint(password[2]);
    printint(password[3]);


    while(1){
        // reset stuff
        int selected = 0;
        int validadas[2] = {0};

        tentativas = 9; // 0 -> 9 = 10
        cursor.x = 20; cursor.y = 0;
        makePassword();

        guess[0] = 1; guess[1] = 1; guess[2] = 1; guess[3] = 1;

        redraw(cursor, guess, 0 ); // initialize the screen
        _delay_ms(10);

        while(tentativas) {

            redraw(cursor, guess, 0);

            char c = keypad_poll();

            if( c == 'T' ){
                if( tryToGuess(validadas) ){
                    print("GANHOU!!!!!");
                    break;
                }
                tentativas--;
                secs_remaining = 30;
                redraw(cursor, guess, selected);
                print("\nPosCorretas: ");
                printint(validadas[0]);
                print(",   PosIncorretas: ");
                printint(validadas[1]);

                _delay_ms(100);
            }

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
        // GAME OVER
        nokia_lcd_clear();
        nokia_lcd_set_cursor(0, 0);
        if(tentativas <= 0){
            nokia_lcd_write_string("GAME", 3);
            nokia_lcd_set_cursor(0, 24);
            nokia_lcd_write_string("OVER", 3);
        }

        else {
            nokia_lcd_write_string("YOU", 3);
            nokia_lcd_set_cursor(0, 24);
            nokia_lcd_write_string("WON", 3);
        }
        nokia_lcd_render();
        _delay_ms(3000);
    }

}