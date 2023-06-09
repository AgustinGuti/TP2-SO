#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include <stdint.h>
#include "scheduler.h"
#include "pipes.h"

// Forward declaration for Pipe type
typedef struct PipeCDT PipeCDT;
typedef PipeCDT *Pipe;

#define BACKSPACE 0x08
#define NEWLINE 0x0A
#define TAB 0x09

#define DEFAULT_LETTER_WHITE 0x00FFFFFF // White letters
#define DEFAULT_BACK_BLACK              // Black backgound

char hasKeyReady();

// Returns the ASCII code (decimal) of the character obtained
int getKeyMake(uint8_t event);

char isKeyBreak(unsigned char data);

char isKeyMake(unsigned char data);

void keyboard_handler(uint8_t event);
Pipe getKeyboardBuffer();
void closeKeyboardBuffer();

int getBuffer(int *out, uint32_t count);

#endif