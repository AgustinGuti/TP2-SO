// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <videoDriver.h>

#define WHITE 0xFFFFFF

#define MAX_FONT_SIZE 4
#define MAX_CHAR_BUFFER 6144

extern uint8_t *memmov(uint64_t destination, uint64_t source, uint64_t lenght);
extern uint8_t screenBuffer[1024 * 8 * 100];

struct vbe_mode_info_structure
{
	uint16_t attributes;	// deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8_t window_a;			// deprecated
	uint8_t window_b;			// deprecated
	uint16_t granularity; // deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr; // deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t pitch;				 // number of bytes per horizontal line
	uint16_t width;				 // width in pixels
	uint16_t height;			 // height in pixels
	uint8_t w_char;				 // unused...
	uint8_t y_char;				 // ...
	uint8_t planes;
	uint8_t bpp;	 // bits per pixel in this mode
	uint8_t banks; // deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size; // deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8_t image_pages;
	uint8_t reserved0;

	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;

	uint32_t framebuffer; // physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size; // size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
} __attribute__((packed));

struct vbe_mode_info_structure *vbeInfo = (void *)0x5C00; // VBEModeInfoBlock starting direction (sysvar.asm)

static uint8_t *currentCharOffset;
static char pageStarted = 0;
static uint8_t fontSize = 1; // Default is 1

static uint8_t characterBuffer[MAX_CHAR_BUFFER]; // Stores all chars printed to screen. Size is considered for a 1024x768, and minimun size of characters 8*16
																								 // Size = charsPerLine * lineQty = width/CHAR_WIDTH * height/CHAR_HEIGHT = 6144 characters
static uint16_t lastChar = 0;										 // Position markers for characterBuffer to work as a circular array
static uint16_t firstChar = 0;
static char reprinting = 0; // Boolean value to stop the characterBuffer filling

uint16_t getScreenWidth()
{
	return vbeInfo->width;
}

uint16_t getScreenHeight()
{
	return vbeInfo->height;
}

uint16_t getScreenPitch()
{
	return vbeInfo->pitch;
}

uint32_t getScreenFrameBuffer()
{
	return vbeInfo->framebuffer;
}

uint8_t getScreenBpp()
{
	return vbeInfo->bpp;
}

uint32_t getPixelOffset(pxlCoord coord)
{
	uint16_t pitch = getScreenPitch();
	uint32_t framebuffer = getScreenFrameBuffer();
	uint8_t bpp = getScreenBpp();
	return (coord.y * pitch + coord.x * (bpp / 8) + framebuffer);
}

void setFontSize(uint8_t size, uint8_t rewrite)
{
	if (size > 0 && size <= MAX_FONT_SIZE)
	{ // Size can't be 0 or greater than 4
		fontSize = size;
	}
	if (rewrite)
	{
		reprintFromBuffer(); // This is needed to update older characters on screen
	}
}

uint8_t getFontSize()
{
	return fontSize;
}

void cleanScreen()
{
	drawRect((pxlCoord){0, 0}, 0, getScreenWidth(), getScreenHeight());
	startPage();
}

void clearCharBuffer()
{
	firstChar = lastChar = 0; // If clean screen is called
}

void drawRect(pxlCoord coord, uint32_t color, uint16_t width, uint16_t height)
{
	if (coord.x + width > getScreenWidth() || coord.y + height > getScreenHeight())
	{ // Check screen bounds
		return;
	}
	uint8_t bpp = getScreenBpp();
	uint16_t pitch = getScreenPitch();
	uint8_t *pixelOffset = getPixelOffset(coord);

	// Stores a line in the memory buffer
	for (int i = 0; i < width; i++)
	{
		for (int k = 0; k < bpp / 8; k++)
		{
			screenBuffer[i * (bpp / 8) + k] = (color >> (k * 8)) & 255;
		}
	}
	// Copy lines
	for (int i = 0; i < height; i++)
	{
		memmov(pixelOffset, screenBuffer, width * (bpp / 8));
		pixelOffset += pitch;
	}
}

void startPage()
{
	currentCharOffset = getPixelOffset((pxlCoord){0, 0});
	pageStarted = 1;
}

void printStringLimited(uint32_t color, uint8_t *str, uint32_t count)
{
	if (pageStarted == 0)
	{
		startPage();
	}
	int i = 0;
	while (str[i] != 0 && i < count)
	{
		putChar(color, str[i++]);
	}
}

void printString(uint32_t color, uint8_t *str)
{
	if (pageStarted == 0)
	{
		startPage();
	}
	int i = 0;
	while (str[i] != 0)
	{
		putChar(color, str[i++]);
	}
}

// Can receive %s, %c, %d, %X, %x
//  %X and %x have the same behaviour
void printFormatString(uint32_t color, va_list valist, uint32_t argQty, const char *fmt)
{
	int pos = 0;
	int consumedArgs = 0;
	while (fmt[pos] != 0)
	{
		// If all arguments are consumed it stops searching (va_arg has undefined behaviour)
		if (fmt[pos] != '%' || consumedArgs == argQty)
		{
			putChar(color, fmt[pos]);
		}
		else
		{
			consumedArgs++;
			pos++;
			char type = fmt[pos];
			if (type == 's')
			{
				printString(color, va_arg(valist, int));
			}
			else if (type == 'c')
			{
				putChar(color, va_arg(valist, int));
			}
			else if (type == 'd')
			{
				int num = va_arg(valist, int);
				int numLength = dec_num_length(num);
				if (num < 0)
				{
					numLength++;
				}
				char aux[numLength + 1];
				dec_to_str(aux, num);
				printString(color, aux);
			}
			else if (type == 'X' || type == 'x')
			{
				uint64_t num = va_arg(valist, uint64_t);
				int numLength = hex_num_length(num);
				char aux[numLength + 1];
				hex_to_str(aux, num);
				printString(color, aux);
			}
			else
			{ // If it is an unrecognized format, it prints it literally
				consumedArgs--;
				putChar(color, '%');
				putChar(color, type);
			}
		}
		pos++;
	}
}

// Can receive %s, %c, %d, %X, %x
//  %X and %x have the same behaviour
void printf(const char *fmt, ...)
{
	va_list valist;
	int pos = 0;
	int argQty = 0;
	while (fmt[pos] != 0)
	{
		if (fmt[pos] == '%' && fmt[pos + 1] != 0)
		{
			argQty++;
		}
		pos++;
	}
	va_start(valist, fmt);
	printFormatString(0xFFFFFF, valist, argQty, fmt);
	va_end(valist);
}

// Can receive %s, %c, %d, %X, %x
//  %X and %x have the same behaviour
void printerr(const char *fmt, ...)
{
	va_list valist;
	int pos = 0;
	int argQty = 0;
	while (fmt[pos] != 0)
	{
		if (fmt[pos] == '%' && fmt[pos + 1] != 0)
		{
			argQty++;
		}
		pos++;
	}
	va_start(valist, fmt);
	printFormatString(0xFF0000, valist, argQty, fmt);
	va_end(valist);
}

void putChar(uint32_t color, uint16_t id)
{
	if (id < FIRST_CHAR || id > FIRST_CHAR + CHAR_QTY)
	{ // Only mapped ASCII
		if (id != NEWLINE && id != BACKSPACE)
		{ // Also accepts NEWLINE and BACKSPACE
			return;
		}
	}
	if (pageStarted == 0)
	{
		startPage();
	}
	if (!reprinting && id != BACKSPACE)
	{ // Don't insert backspaces
		characterBuffer[lastChar++] = id;
		if (lastChar == MAX_CHAR_BUFFER)
		{
			lastChar = 0;
		}
		if (lastChar == firstChar)
		{ // Oldest characters are deleted
			firstChar++;
		}
		if (firstChar == MAX_CHAR_BUFFER)
		{
			firstChar = 0;
		}
	}
	uint8_t bpp = getScreenBpp();
	uint16_t pitch = getScreenPitch();

	uint8_t bytePerPixel = bpp / 8;

	uint8_t charPixelWidth = CHAR_WIDTH * fontSize;
	uint8_t charByteWidth = charPixelWidth * bytePerPixel;

	uint16_t charsPerLine = pitch / charByteWidth;
	uint64_t currentCharOffsetToStart = currentCharOffset - getScreenFrameBuffer();

	uint16_t charsToEndOfLine = (charsPerLine - (currentCharOffsetToStart % pitch) / charByteWidth) - 1;
	switch (id)
	{
	case NEWLINE:
		newline();
		break;
	case BACKSPACE:
		lastChar--; // Remove the last inserted letter
		if (lastChar < 0)
		{
			lastChar = MAX_CHAR_BUFFER - 1;
		}
		backspace();
		lastChar--; // Remove the space inserted to erase the letter
		if (lastChar < 0)
		{
			lastChar = MAX_CHAR_BUFFER - 1;
		}
		break;
	default:
		if (charsToEndOfLine == 0 && currentCharOffsetToStart != 0)
		{ // Check if another character fits
			newline();
		}
		for (int i = 0; i < CHAR_HEIGHT * fontSize; i++)
		{
			for (int j = 0; j < charPixelWidth; j++)
			{
				for (int k = 0; k < bytePerPixel; k++)
				{																																		 // We are in 24 bit mode, but this should be valid for different bpp
					uint16_t charHorizontalLine = font[id - FIRST_CHAR][i / fontSize]; // i-nth horizontal line of the glyph
					uint8_t charVerticalPos = CHAR_WIDTH - 1 - j / fontSize;					 // j-nth position of the line, starting from the end
					currentCharOffset[j * bytePerPixel + k] = getBit(charHorizontalLine, charVerticalPos) * (color >> (k * 8) & 255);
				}
			}
			currentCharOffset += pitch;
		}
		currentCharOffset += charByteWidth;
		currentCharOffset -= pitch * CHAR_HEIGHT * fontSize;
		break;
	}
}

void printFormatStringLimited(uint32_t color, const char *str, uint32_t count, uint16_t row, uint16_t col)
{
	if (pageStarted == 0)
	{
		startPage();
	}
	uint8_t *aux = currentCharOffset;
	currentCharOffset = getPixelOffset((pxlCoord){col * CHAR_WIDTH * fontSize, row * CHAR_HEIGHT * fontSize});
	int i = 0;
	printStringLimited(color, str, count);
	currentCharOffset = aux;
}

/*
	Draws width * height * bpp/8 bytes from sprite to the screen, begining in coord.
	Width and height should be in pixel size
*/
void drawSprite(uint16_t width, uint16_t height, uint8_t sprite[height][width * getScreenBpp() / 8], pxlCoord coord)
{
	if (coord.x + width > getScreenWidth() || coord.y + height > getScreenHeight())
	{ // Check screen bounds
		return;
	}
	uint8_t *pixelOffset = getPixelOffset(coord);
	uint16_t pitch = getScreenPitch();
	uint8_t bytesPerPixel = getScreenBpp() / 8;
	for (int i = 0; i < height; i++)
	{
		memmov(pixelOffset + i * pitch, sprite[i], width * bytesPerPixel);
	}
}

void backspace()
{
	uint8_t bpp = getScreenBpp();
	uint16_t pitch = getScreenPitch();
	currentCharOffset -= CHAR_WIDTH * fontSize * (bpp / 8);
	putChar(0x000000, ' ');
	currentCharOffset -= CHAR_WIDTH * fontSize * (bpp / 8);
}

void rollUp()
{
	uint16_t deletedAmount = 0;
	uint16_t charsPerLine = getScreenPitch() / (CHAR_WIDTH * (getScreenBpp() / 8));
	while (deletedAmount < charsPerLine && characterBuffer[firstChar] != NEWLINE)
	{
		deletedAmount++;
		firstChar++;
		if (firstChar == MAX_CHAR_BUFFER)
		{
			firstChar = 0;
		}
	}
	if (characterBuffer[firstChar] == NEWLINE)
	{
		firstChar++;
	}
	if (firstChar == MAX_CHAR_BUFFER)
	{
		firstChar = 0;
	}

	uint16_t pitch = getScreenPitch();
	uint16_t screenWidth = getScreenWidth();
	uint16_t charPixelHeight = CHAR_HEIGHT * fontSize;

	uint32_t nextLineDelta = pitch * charPixelHeight;
	uint64_t screenFrameBuffer = getScreenFrameBuffer();

	// Much faster than reprinting the whole screen.
	memmov(screenFrameBuffer, screenFrameBuffer + nextLineDelta, getScreenHeight() * pitch - nextLineDelta);

	uint64_t currentCharOffsetToStart = currentCharOffset - screenFrameBuffer;
	uint16_t currentRow = currentCharOffsetToStart / pitch;

	currentCharOffset = currentRow * pitch + screenFrameBuffer;

	// Stores a black line in memory buffer
	for (int i = 0; i < pitch; i++)
	{
		screenBuffer[i] = 0;
	}
	// Clear last line
	for (int i = 0; i < charPixelHeight; i++)
	{
		memmov(currentCharOffset + i * pitch, screenBuffer, pitch);
	}
}

void reprintFromBuffer()
{
	reprinting = 1; // Don't store this characteres in the bufferr
	cleanScreen();
	startPage();
	int i = firstChar;
	while (i != lastChar)
	{
		if (i == MAX_CHAR_BUFFER)
		{
			i = 0;
		}
		putChar(WHITE, characterBuffer[i++]);
	}
	reprinting = 0;
}

void newline()
{
	uint64_t currentCharOffsetToStart = currentCharOffset - getScreenFrameBuffer();

	uint16_t screenHeight = getScreenHeight();
	uint8_t charPixelHeight = CHAR_HEIGHT * fontSize;

	uint16_t linesPerPage = screenHeight / charPixelHeight - 1;
	uint16_t currentRow = currentCharOffsetToStart / getScreenPitch() / charPixelHeight;

	if (currentRow + 1 > linesPerPage)
	{
		rollUp();
	}
	else
	{
		currentCharOffset = (currentRow + 1) * getScreenPitch() * charPixelHeight + getScreenFrameBuffer();
	}
}