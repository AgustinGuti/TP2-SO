// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <keyboard.h>
#include <semaphores.h>
#include <videoDriver.h>
#include <pipes.h>

extern void _hlt();
extern char has_key();
extern char get_key();

// Special characters must be greater than 255 in order not to clash with extended ASCII characters
#define BLOCK_MAYUS 260
#define LSHIFT 261
#define RSHIFT 262 // NOT IMPLEMENTED
#define LCTRL 263
#define LALT 264
#define HOME 265
#define PAGE_UP 266
#define END 267
#define PAGE_DOWN 268
#define INSERT 269
#define CTRL_C 270
#define EOF 271
#define DELETE 127

#define LSHIFT_BREAK 0xAA
#define RSHIFT_BREAK 0xB6
#define LCTRL_BREAK 0x9D
#define LALT_BREAK 0xB8

// Ctrl when shell is running
#define LCTRL_MAKE 0x1D

#define UP_ARROW 300
#define DOWN_ARROW 301
#define LEFT_ARROW 302
#define RIGHT_ARROW 303

#define ESCAPE 27

#define MAX_MAPPED 58

#define MIN_SPECIAL_MAPPED 0xE047
#define MAX_SPECIAL_MAPPED 0xE053

// Mapping of ASCII value of characters

// mapped for english keyboard
// lowercase map
static const int16_t keyMapping[] =
    // 00,   01, 02  ,03  ,04   ,05  ,06,   07, 08, 09,   0A, 0B,   0C,  0D,     0E,    0F,  10, 11, 12, 13, 14, 15, 16, 17, 18 ,19, 1A, 1B, 1C, 1D,
    {-1, ESCAPE, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', BACKSPACE, TAB, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', NEWLINE, LCTRL,
     // 1E,1F ,20 ,21 ,22 ,23 ,24 ,25 ,26 ,27 ,28  ,29,  2A,  2B,2C ,2D ,2E ,2F ,30 ,31 ,32 ,33 ,34 ,35 ,  36,  37, 8,  39,  3A
     'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '|', LSHIFT, -1, 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', RSHIFT, -1, LALT, ' ', BLOCK_MAYUS};

// uppercase map
static const int16_t shiftKeyMapping[] =
    // 00,01   , 02 ,03 ,04 ,05 ,06 ,07 ,08 ,09 ,0A ,0B ,0C ,0D  ,    0E,    0F, 10,11 ,12 ,13 ,14 ,15 ,16 ,17 ,18 ,19 ,1A ,1B ,  1C,     1D,
    {-1, ESCAPE, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', BACKSPACE, TAB, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', NEWLINE, LCTRL,
     // 1E,1F ,20 ,21 ,22 ,23 ,24 ,25 ,26 ,27 ,28  ,29,  2A,  2B,2C ,2D ,2E ,2F ,30 ,31 ,32 ,33 ,34 ,35 ,  36,  37, 38, 39,  3A
     'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', -1, LSHIFT, -1, 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', RSHIFT, -1, LALT, ' ', BLOCK_MAYUS};

static char isShiftDown = 0;
static char isCtrDown = 0;
static char isAltDown = 0;

static const int specialKeyMapping[] =
    // E0 47, EO 48,      E0 49,    EO 4A,      E0 4B,     EO 4C,     E0 4D,    EO 4E,  E0 4F,    EO 50,        E0 51,      EO 52, EO 53
    {7, UP_ARROW, PAGE_UP, -1, LEFT_ARROW, -1, RIGHT_ARROW, -1, END, DOWN_ARROW, PAGE_DOWN, INSERT, DELETE};

// Returns 0 if data is not mapped, 1 if it is
char isMapped(unsigned char data)
{
    return data <= MAX_MAPPED && keyMapping[data] >= 0;
}

// Returns 0 if data is not mapped, 1 if it is
char isSpecialMapped(uint16_t data)
{
    return specialKeyMapping[data] >= 0;
}

char isSpecialKeyMake(uint16_t data)
{
    if (data >= MIN_SPECIAL_MAPPED && data <= MAX_SPECIAL_MAPPED)
    {
        return 1;
    }
    return 0;
}

// Return ASCII code of char received, or -1 if a char wasn't received
int getKeyMake(uint8_t event)
{
    uint16_t specialEvent = 0;
    switch (event)
    {
    case LSHIFT_BREAK:
        isShiftDown = 0;
        break;
    case LCTRL_BREAK:
        isCtrDown = 0;
        break;
    case LALT_BREAK:
        isAltDown = 0;
        break;
    case LCTRL_MAKE:
        isCtrDown = 1;
        break;
    default:
        if (event == 0xE0)
        { // 2 part command
            specialEvent = event << 8;
            specialEvent += get_key();
        }
        if (isKeyMake(event) && isMapped(event))
        {
            uint16_t key = keyMapping[event];
            switch (key)
            {
            case LSHIFT:
                isShiftDown = 1;
                break;
            case LALT:
                isAltDown = 1;
                break;
            default:
                if (key < 255)
                { // Ignores other special characters
                    if (isShiftDown)
                    {
                        return shiftKeyMapping[event];
                    }
                    if (isCtrDown && key == 'c')
                    {
                        return CTRL_C;
                    }
                    if (isCtrDown && key == 'd')
                    {
                        return EOF;
                    }
                    return keyMapping[event];
                }
                return -1;
            }
        }
        else if (isSpecialKeyMake(specialEvent) && isSpecialMapped(specialEvent))
        {
            return specialKeyMapping[specialEvent - MIN_SPECIAL_MAPPED];
        }
    }
    return -1;
}

char isKeyBreak(unsigned char data)
{
    // If code is greater than 0x79, then it is a break
    if (data > 0x79)
    {
        return 1;
    }
    return 0;
}

char isKeyMake(unsigned char data)
{
    if (data < 0x79)
    {
        return 1;
    }
    return 0;
}

#define BUFFER_SIZE 1024

static Pipe buffer = NULL;

void keyboard_handler(uint8_t event)
{
    if (buffer == NULL)
    {
        buffer = openPipe(NULL);
    }
    int key = getKeyMake(event);
    if (key != -1)
    {
        char newChar[1];
        switch (key)
        {
        case TAB:
            writeToPipe(buffer, "   ", 3);
            break;
        case NEWLINE:
            newChar[0] = NEWLINE;
            writeToPipe(buffer, newChar, 1);
            break;
        case CTRL_C:
            killForegroundProcess();
            break;
        case EOF:
            newChar[0] = -1;
            writeToPipe(buffer, newChar, 1);
            break;
        default:
            newChar[0] = (char)key;
            writeToPipe(buffer, newChar, 1);
            break;
        }
    }
}

Pipe getKeyboardBuffer()
{
    if (buffer == NULL)
    {
        buffer = openPipe(NULL);
    }
    return buffer;
}

void closeKeyboardBuffer()
{
    if (buffer != NULL)
    {
        closePipe(buffer);
        buffer = NULL;
    }
}