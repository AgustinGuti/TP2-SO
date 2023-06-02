// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

int Hex2Int(const char p)
{
    if (p <= '9')
    {
        return p - '0';
    }
    else if (p <= 'F')
    {
        return p - 'A' + 10;
    }
    else
    {
        return p - 'a' + 10;
    }
}

int readBdf(FILE *bdf, FILE *out, FILE *outHeader)
{
    char *aux;
    uint8_t *token;
    char linebuf[1024];
    int fontboundingbox_width = 0;
    int fontboundingbox_height = 0;
    int fontboundingbox_xoff = 0;
    int fontboundingbox_yoff = 0;
    int charQty;
    char charname[1024];
    int encoding;
    int bbx;
    int bby;
    int bbw;
    int bbh;
    int currentChar = 0;

    uint8_t value;

    char *readChar = fgets(linebuf, sizeof(linebuf), bdf);

    // Leo la info que me interesa del header
    while (readChar && (aux = strtok(linebuf, " \t\n\r")))
    {
        if (strcasecmp(aux, "FONTBOUNDINGBOX") == 0)
        {
            token = strtok(NULL, " \t\n\r");
            fontboundingbox_width = atoi(token);
            token = strtok(NULL, " \t\n\r");
            fontboundingbox_height = atoi(token);
            token = strtok(NULL, " \t\n\r");
            fontboundingbox_xoff = atoi(token);
            token = strtok(NULL, " \t\n\r");
            fontboundingbox_yoff = atoi(token);
        }
        else if (strcasecmp(aux, "CHARS") == 0)
        {
            token = strtok(NULL, " \t\n\r");
            charQty = atoi(token);
            break; // Ya termino el header
        }
        readChar = fgets(linebuf, sizeof(linebuf), bdf);
    }

    uint8_t varSize = fontboundingbox_width;
    uint8_t varSizeAsm;
    if (varSize <= 8)
    {
        varSize = 8;
        varSizeAsm = 'b'; // byte
    }
    else if (varSize <= 16)
    {
        varSize = 16;
        varSizeAsm = 'w'; // word
    }
    else if (varSize <= 32)
    {
        varSize = 32;
        varSizeAsm = 'd'; // dword
    }
    else if (varSize <= 64)
    {
        varSize = 64;
        varSizeAsm = 'q'; // qword
    }
    else
    { // Too big
        return -1;
    }

    fprintf(out, "GLOBAL font \n\n");
    fprintf(out, "section .data \n");
    fprintf(out, "font:\n");
    //   fprintf(out, "  font d%c ",varSizeAsm);
    fprintf(outHeader, "#include <stdint.h> \n");
    fprintf(outHeader, "#define CHAR_HEIGHT %d\n", fontboundingbox_height);
    fprintf(outHeader, "#define CHAR_WIDTH %d\n", fontboundingbox_width);

    int isFirst = 1;

    // Leo la info de los caracteres
    while (readChar && (aux = strtok(linebuf, " \t\n\r")))
    {
        if (strcasecmp(aux, "STARTCHAR") == 0)
        {
            token = strtok(NULL, "\t\n\r");
            strcpy(charname, token);
        }
        else if (strcasecmp(aux, "ENCODING") == 0)
        { // ASCII
            token = strtok(NULL, " \t\n\r");
            encoding = atoi(token);
            if (isFirst)
            {
                fprintf(outHeader, "#define FIRST_CHAR %d\n", encoding);
                isFirst = 0;
            }
        }
        else if (strcasecmp(aux, "BBX") == 0)
        {
            token = strtok(NULL, " \t\n\r");
            bbw = atoi(token);
            token = strtok(NULL, " \t\n\r");
            bbh = atoi(token);
            token = strtok(NULL, " \t\n\r");
            bbx = atoi(token);
            token = strtok(NULL, " \t\n\r");
            bby = atoi(token);
        }
        else if (strcasecmp(aux, "BITMAP") == 0)
        {
            fprintf(out, "  font%d d%c ", currentChar, varSizeAsm);
            for (int j = 0; j < fontboundingbox_height; j++)
            {
                readChar = fgets(linebuf, sizeof(linebuf), bdf);
                // aux = strtok(linebuf, " \t\n\r");
                token = strtok(linebuf, " \t\n\r"); // ?
                uint32_t num = 0;
                for (int k = 0; k < fontboundingbox_width / 4; k++)
                {
                    num = num << 4;
                    num += Hex2Int(token[k]);
                }
                fprintf(out, "0x%0*X", fontboundingbox_width / 4, num);
                if (j != fontboundingbox_height - 1)
                {
                    fprintf(out, ", ");
                }
            }
            fprintf(out, "; %d $%02x '%s'    ", encoding, encoding, charname);
            fprintf(out, " bbx %d, bby %d, bbw %d, bbh %d", bbx, bby, bbw, bbh);
            fprintf(out, "\n");
            currentChar++;
        }
        if (encoding > 127)
        { // Ignores not ascii characters
            break;
        }
        readChar = fgets(linebuf, sizeof(linebuf), bdf);
    }
    fprintf(outHeader, "#define CHAR_QTY %d\n\n", currentChar);
    fprintf(outHeader, "extern const uint%d_t font[CHAR_QTY][CHAR_HEIGHT];\n", varSize);
}

int main()
{
    FILE *in = fopen("spleen-8x16.bdf", "r");
    FILE *outHeader = fopen("../Kernel/include/fonts.h", "w");
    FILE *out = fopen("../Kernel/asm/fonts.asm", "w");

    readBdf(in, out, outHeader);

    return 0;
}