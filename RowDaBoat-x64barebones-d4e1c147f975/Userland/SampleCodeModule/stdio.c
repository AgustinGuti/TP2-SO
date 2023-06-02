// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include <stddef.h>

#define LINE_INDICATOR "$> "
#define EOF -1

void newlineFD(int fd);
void backspaceFD(int fd);
void printTextFD(char *string, int fd);
void putCharFD(char character, int fd);
void printFD(int fd, const char *fmt, int argQty, va_list valist);

// Can receive %s, %c, %d, %X, %x
//%X and %x have the same behaviour
// Accepts \n as command, (can't escape the \)
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
	printFD(STDOUT, fmt, argQty, valist);
	va_end(valist);
}

void printerr(const char *fmt, int argQty, ...)
{
	// va_list valist;
	// va_start(valist, argQty);
	// printFD(STDERR, fmt, argQty, valist);
	// va_end(valist);
}

void printFD(int fd, const char *fmt, int argQty, va_list valist)
{
	int pos = 0;
	int consumedArgs = 0;
	while (fmt[pos] != 0)
	{
		// If all arguments are consumed it stops searching (va_arg has undefined behaviour)
		if (fmt[pos] != '%' || consumedArgs == argQty)
		{
			if (fmt[pos] == '\\' && fmt[pos + 1] == 'n')
			{ // Interprets \n as newline
				newlineFD(fd);
			}
			putCharFD(fmt[pos], fd);
		}
		else
		{
			consumedArgs++;
			pos++;
			char type = fmt[pos];
			if (type == 's')
			{
				// int i = 0;
				// char *string = va_arg(valist, char *);
				// while(string[i] != 0){
				// 	putCharFD(string[i],fd);
				// 	i++;
				// }
				printTextFD(va_arg(valist, char *), fd);
			}
			else if (type == 'c')
			{
				putCharFD(va_arg(valist, int), fd);
			}
			else if (type == 'd')
			{
				int num = va_arg(valist, int);
				int numLength = decNumLength(num);
				char aux[numLength + 1];
				decToStr(aux, num);
				printTextFD(aux, fd);
			}
			else if (type == 'X' || type == 'x')
			{
				long num = va_arg(valist, long);
				int numLength = hexNumLength(num);
				char aux[numLength + 1];
				hexToStr(aux, num);
				printTextFD(aux, fd);
			}
			else
			{ // If undefined format, prints it literally
				consumedArgs--;
				putCharFD('%', fd);
				putCharFD(type, fd);
			}
		}
		pos++;
	}
}

void printTextFD(char *string, int fd)
{
	_sys_write(fd, string, strlen(string));
}

void printText(char *string)
{
	printTextFD(string, STDOUT);
}

void putCharFD(char character, int fd)
{
	char aux[] = {character};
	_sys_write(fd, aux, 1);
}

void putChar(char character)
{
	putCharFD(character, STDOUT);
}

int getChar()
{
	char buf[1] = {0};
	char c = _sys_read(0, buf, 1);
	if (c != 1)
	{
		return EOF;
	}
	return buf[0];
}

void newlineFD(int fd)
{
	putCharFD(NEWLINE, fd);
}

void backspaceFD(int fd)
{
	putCharFD(BACKSPACE, fd);
}

void newline()
{
	newlineFD(STDOUT);
}

void backspace()
{
	backspaceFD(STDOUT);
}

char cleanScreen(char argc, char **argv)
{
	_sys_cleanScreen();
	return 0;
}

// Accepts format = "%[count][format]", reads up to count chars, up to a 0 terminating the string. No [count] argument reads until 0.
// Returns amount of char read.
// Formats:
//  %s reads standard ascii characters. Ignores values over UINT8_MAX and below 0
//  %S accepts special characters. Ignores values over UINT16_MAX and below 0
int scanf(const char *fmt, int argQty, ...)
{
	int pos = 0;
	va_list valist;
	va_start(valist, argQty);
	uint32_t totalReadChars = 0;
	int consumedArgs = 0;
	while (fmt[pos] != 0)
	{
		// If all arguments are consumed, stop accepting new argumetns. (va_arg has undefined behaviour if polled for more elements that it has)
		if (fmt[pos++] == '%' && consumedArgs != argQty)
		{
			consumedArgs++;
			// Amount of digits following %
			int i = pos;
			while (fmt[i] >= '0' && fmt[i] <= '9')
			{
				i++;
			}
			i -= pos;
			int charQty = 0;
			if (i != 0)
			{
				char charQtyStr[i];
				for (int j = 0; j < i; j++)
				{
					charQtyStr[j] = fmt[j + pos];
				}
				charQty = strToNum(charQtyStr, i);
				pos += i;
			}

			char type = fmt[pos];
			if (type == 's')
			{
				uint8_t *out = va_arg(valist, char *);
				char aux[1] = {0};
				int readChar = 1;
				int arrPos = 0;
				while (readChar != 0 && (arrPos < charQty || i == 0))
				{																	 // Nothing else to read, or reached max char quantity. If there was no max, read to the end
					readChar = _sys_read(0, aux, 1); 
					if (aux[0] >= 0)
					{
						out[arrPos++] = aux[0];
						totalReadChars += readChar;
					}
				}
			}
			else if (type == 'S')
			{
				uint16_t *out = va_arg(valist, char *);
				char aux[1] = {0};
				int readChar = 1;
				int arrPos = 0;

				while (readChar != 0 && (arrPos < charQty || i == 0))
				{																	 // Nothing else to read, or reached max char quantity. If there was no max, read to the end
					readChar = _sys_read(0, aux, 1); // STDIN, first reads into an auxiliar character to make sure it is not too big
					if (aux[0] > 0)
					{
						out[arrPos++] = aux[0];
						totalReadChars += readChar;
					}
					aux[0] = 0;
				}
			}
		}
		pos++;
	}
	va_end(valist);
	return totalReadChars;
}

void sendEOF()
{
	char buf[1] = {EOF};
	_sys_write(1, buf, 1);
}

void formatPrint(const char *fmt, uint32_t color, uint16_t row, uint16_t col, int argQty, ...)
{
	int pos = 0;
	va_list valist;
	va_start(valist, argQty);
	int consumedArgs = 0;
	char buf[1024];
	int j = 0;
	while (fmt[pos] != 0)
	{
		// If all arguments are consumed, stop accepting new argumetns. (va_arg has undefined behaviour if polled for more elements that it has)
		if (fmt[pos] != '%' || consumedArgs == argQty)
		{
			if (fmt[pos] == '\\' && fmt[pos + 1] == 'n')
			{ // Interprets \n as newline
				newline();
			}
			buf[j++] = fmt[pos];
		}
		else
		{
			consumedArgs++;
			pos++;
			char type = fmt[pos];
			if (type == 's')
			{
				char *aux = va_arg(valist, int);
				for (int i = 0; aux[i] != 0; i++)
				{
					buf[j++] = aux[i];
				}
			}
			else if (type == 'c')
			{
				buf[j++] = va_arg(valist, int);
			}
			else if (type == 'd')
			{
				int num = va_arg(valist, int);
				int numLength = decNumLength(num);
				if (num < 0)
				{
					numLength++;
				}
				char aux[numLength + 1];
				decToStr(aux, num);
				for (int i = 0; aux[i] != 0; i++)
				{
					buf[j++] = aux[i];
				}
			}
			else if (type == 'X' || type == 'x')
			{
				int num = va_arg(valist, int);
				int numLength = hexNumLength(num);
				char aux[numLength + 1];
				hexToStr(aux, num);
				for (int i = 0; aux[i] != 0; i++)
				{
					buf[j++] = aux[i];
				}
			}
			else
			{ // If undefined format, prints it literally
				consumedArgs--;
				buf[j++] = '%';
				buf[j++] = type;
			}
		}
		pos++;
	}
	buf[j] = 0;
	printFormatText(buf, color, row, col);
	va_end(valist);
}

void printFormatText(char *string, uint32_t color, uint16_t row, uint16_t col)
{
	_sys_formatWrite(STDOUT, string, strlen(string), color, row, col);
}