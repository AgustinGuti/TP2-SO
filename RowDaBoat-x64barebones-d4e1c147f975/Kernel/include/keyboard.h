#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include <stdint.h>

#define BACKSPACE   0x08
#define NEWLINE     0x0A
#define TAB         0x09

#define DEFAULT_LETTER_WHITE 0x00FFFFFF 		//White letters
#define DEFAULT_BACK_BLACK			//Black backgound
#define MAX_BUFFER_LENGTH 64

char hasKeyReady();

//Devuelve el codigo ASCII (decimal) del caracter obtenido
uint16_t getKeyMake(uint8_t event);

char isKeyBreak(unsigned char data);

char isKeyMake(unsigned char data);

void keyboard_handler(uint8_t event);
void appendToBuffer(char letter);

//void backspace();
//void printLine();

int getBuffer(int* out, uint32_t count);
void removeFromBuffer(uint32_t count);

#endif