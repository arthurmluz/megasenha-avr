
#include <stdio.h>
#include <avr/io.h>
#include <stdlib.h>
#include <ctype.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <time.h>
#include "nokia5110.h"
#include "keypad.h"
#include "print.h"
#include "./shift_reg.h"

#define RIGHT '>'
#define LEFT '<'
#define SELECT 'v' 
#define TRY 'T'
#define SYMBOL_COUNT 9

uint8_t glyph[] = { 0b00111000,0b01101000,0b11001000,0b01101000,0b00111000 };

uint8_t glyph2[] = {0b00111000,0b01111000,0b11111000,0b01111000,0b00111000 };

uint8_t heart_outline[] = {0b00011000,0b00100100,0b01001000,0b00100100,0b00011000 };
uint8_t heart_fill[] = {0b00011000,0b00111100,0b01111000,0b00111100,0b00011000 };

uint8_t hourglass[] = {
    0b01100011,
    0b01010101,
    0b01101011,
    0b01010101,
    0b01100011 };

typedef struct sel{
    int x, y;
} select;

// global variables YES, global. I know what i'm doing :)
int password[4];
int guess[4];
select cursor = {.x = 20, .y = 0};
int max_lives = 10;
int lives = 10;
int secs_remaining = 30;

void initializer();
void escreveVetor(int vetor[]);
void makePassword();
void redraw(select cursor, int guess[]);
void moveCursor(char value);
int tryToGuess(int validadas[2]);
void draw_game_over();
void draw_you_won();
void setup_timer();
void light_leds();

void initializer(){
    nokia_lcd_init();
    nokia_lcd_clear();
    nokia_lcd_custom(1,glyph);
    nokia_lcd_custom(2,glyph2);
    nokia_lcd_custom(3,heart_outline);
    nokia_lcd_custom(4,heart_fill);
    nokia_lcd_custom(5,hourglass);

    setup_timer();
    sei();

    shift_reg_init();
    shift_reg_write(0b11111111);


    keypad_init();
}

void escreveVetor(int vetor[]){
    char buffer[100];
    nokia_lcd_set_cursor(20, 12);
    snprintf(buffer, sizeof(buffer), "%d %d %d %d", vetor[0], vetor[1], vetor[2], vetor[3]);
    nokia_lcd_write_string(buffer, 1);
}

void makePassword(){
    for(int i = 0; i < 4; i++){
        password[i] = 1 + rand() % SYMBOL_COUNT;
        for(int j = 0; j < i; j++){
            if(password[i] == password[j]){
                i--; // se diminuir o i, a gente volta pra mesma posição
                break;
            }
        }
    }
}

void redraw(select cursor, int guess[]){
    nokia_lcd_clear();
    escreveVetor(guess);
    nokia_lcd_set_cursor(cursor.x, cursor.y);

    // Cursor
    nokia_lcd_write_string("\001", 1);

    // Tempo
    char secs_remaining_str[3];
    itoa(secs_remaining, secs_remaining_str, 10);

    nokia_lcd_set_cursor(4, 40);
    nokia_lcd_write_string("\005", 1);

    nokia_lcd_set_cursor(12, 40);
    nokia_lcd_write_string(secs_remaining_str, 1);

    // Tentativas
    for(int i = 0; i < max_lives; i++) {
        nokia_lcd_set_cursor(50 + (6 * (i % 5)), 30 + (9 * (i / 5)));

        if(i >= max_lives - lives) {
            nokia_lcd_write_string("\004", 1);
        } else {
            nokia_lcd_write_string("\003", 1);
        }
    }


    // nokia_lcd_set_cursor(72, 40);
    // char buffer[100];
    // snprintf(buffer, sizeof(buffer), "%d", lives+1);
    // nokia_lcd_write_string(buffer, 1);



    nokia_lcd_render();
}

void moveCursor(char value){
    switch(value){
        case RIGHT:
            if(cursor.x >= 56)
                cursor.x = 20;
            else cursor.x += 12;
            redraw(cursor, guess);
            _delay_ms(100);
            
            break;

        case LEFT:
            if(cursor.x <= 20)
                cursor.x = 56;
            else cursor.x -= 12;
            redraw(cursor, guess);
            _delay_ms(100);
            
            break;
    }
}

int tryToGuess(int validadas[2]){

    char temp[4] = {0, 0, 0, 0};

    // validadas 0 = Posições Corretas
    // validadas 1 = Posições incorretas
    validadas[0] = 0;
    validadas[1] = 0;


    for(int i = 0; i < 4; i++){
        if(guess[i] == password[i]) {
            temp[i] = 1;
            validadas[0]++;
        }
    }

    for(int i = 0; i < 4; i++) {
        if(temp[i] == 0) {
           for(int j = 0; j < 4; j++) {
               if(temp[j] == 0 && guess[i] == password[j]) {
                   validadas[1]++;
               }
           } 
        }
    }

    if( validadas[0] == 4)
        return 1;

    return 0;
}

void draw_game_over() {
    nokia_lcd_clear();
    nokia_lcd_set_cursor(0, 0);

    nokia_lcd_write_string("GAME", 3);
    nokia_lcd_set_cursor(0, 24);
    nokia_lcd_write_string("OVER", 3);

    nokia_lcd_render();
}

void draw_you_won() {
    nokia_lcd_clear();
    nokia_lcd_set_cursor(0, 0);

    nokia_lcd_write_string("YOU", 3);
    nokia_lcd_set_cursor(0, 24);
    nokia_lcd_write_string("WON", 3);

    nokia_lcd_render();
}

void draw_start() {
    nokia_lcd_clear();

    nokia_lcd_set_cursor(4, 12);
    nokia_lcd_write_string("PRESS ANY KEY", 1);

    nokia_lcd_set_cursor(20, 24);
    nokia_lcd_write_string("TO START", 1);

    nokia_lcd_render();
}

void light_leds(int validadas[2]){
    uint8_t byte = 0;

    for(int i = 0; i < 4; i++){
        byte |= (validadas[0] > i) << i;
    }

    for(int i = 0; i < 4; i++){
        byte |= (validadas[1] > i) << (i + 4);
    }

    shift_reg_write(byte);
}

void flash_leds() {
    for(int i = 0; i < 3; i++){
        shift_reg_write(0xff);
        _delay_ms(250);
        shift_reg_write(0x00);
        _delay_ms(250);
    }
}

ISR(TIMER1_COMPA_vect) {
    secs_remaining -= 1;

    if(secs_remaining <= 0) {
        lives = 0;
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
    initializer();

    while(1){

        unsigned int seed = 0;

        draw_start();

        while(keypad_poll() == 0) {
            seed++;
        }

        srand(seed);

        // reset stuff
        int validadas[2] = {0};
        secs_remaining = 30;

        lives = max_lives;
        cursor.x = 20; cursor.y = 0;
        makePassword();
        print("\n");
        printint(password[0]);
        printint(password[1]);
        printint(password[2]);
        printint(password[3]);

        shift_reg_write(0b00000000);

        guess[0] = 1; guess[1] = 1; guess[2] = 1; guess[3] = 1;

        redraw(cursor, guess); // initialize the screen
        _delay_ms(10);

        while(lives) {

            redraw(cursor, guess);

            char c = keypad_poll();

            if( c == 'T' ){
                if( tryToGuess(validadas) ){
                    print("GANHOU!!!!!");
                    light_leds(validadas);

                    break;
                }
                lives--;
                secs_remaining = 30;
                redraw(cursor, guess);
                print("\nPosCorretas: ");
                printint(validadas[0]);
                print(",   PosIncorretas: ");
                printint(validadas[1]);
                light_leds(validadas);

                _delay_ms(100);
            }

            moveCursor(c);

            c = keypad_poll();            
            int arrayPos = (cursor.x-20) / 12; 
            if( isdigit(c) ){
                guess[arrayPos] = (int) c - '0';
                redraw(cursor, guess);
            }
            
        }

        if(lives <= 0) {
            draw_game_over();
        } else {
            draw_you_won();
            flash_leds();
        }
        _delay_ms(3000);
    }

}