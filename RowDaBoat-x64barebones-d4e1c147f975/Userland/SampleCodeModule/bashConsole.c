#include <bashConsole.h>
#include <stddef.h>
#include <processes.h>
#include <testProcesses.h>

extern void zeroDivision();
extern void displayTime();

char setFontSize( uint8_t argQty, char arguments[argQty], uint8_t rewrite);

char help(uint8_t argumentQty, char arguments[argumentQty]);
char clean(uint8_t argumentQty, char arguments[argumentQty]);
char startTron(uint8_t argumentQty, char arguments[argumentQty]);
char callMemoryDump(uint8_t argumentQty, char arguments[argumentQty]);
char time(uint8_t argumentQty, char arguments[argumentQty]);
char callZeroDivision(uint8_t argumentQty, char arguments[argumentQty]);
char callInvalidOpcode(uint8_t argumentQty, char arguments[argumentQty]);
char callSetFontSize(uint8_t argumentQty, char arguments[argumentQty]);
char exitConsole(uint8_t argumentQty, char arguments[argumentQty]);
char callInforeg(uint8_t argumentQty, char arguments[argumentQty]);
char callHimnoAlegria(uint8_t argumentQty, char arguments[argumentQty]);
char callMalloc(uint8_t argumentQty, char arguments[argumentQty]);
char callFree(uint8_t argumentQty, char arguments[argumentQty]);
char callExec(uint8_t argumentQty, char arguments[argumentQty]);
char callPrintProcesses(uint8_t argumentQty, char arguments[argumentQty]);

#define COMMAND_QTY 15

static char *commandNames[COMMAND_QTY] = {"help","clear","tron","memory-dump","time","zero-division","invalid-opcode","set-font-size","inforeg","exit","himno-alegria","malloc", "free", "exec", "ps"};
static char (*commands[])(uint8_t, char *) = {&help,&clean,&tron,&callMemoryDump,&time,&callZeroDivision,&callInvalidOpcode,&callSetFontSize,&callInforeg,&exitConsole,&callHimnoAlegria,&callMalloc, &callFree, &callExec, &callPrintProcesses}; 
static char *commandDescriptions[COMMAND_QTY] = 
        {"Imprime en pantalla los comandos disponibles. Si el argumento identifica a otro comando, explica su funcionamiento.",
         "Vacia la consola.",
         "Ejecuta el juego \"Tron Light Cycles\" para dos jugadores.",
         "Recibe como parametro una direccion de memoria e imprime los 32 bytes de memoria posteriores a la misma.",
         "Imprime en pantalla la hora del sistema.",
         "Genera la excepcion zero division y muestra en pantalla los registros en el momento del error.",
         "Genera la excepcion invalid opcode y muestra en pantalla los registros en el momento del error.",
         "Permite agrandar o achicar la dimension de del texto en pantalla por argumento.",
         "Imprime el valor de los ultimos registros guardados. Para guardar los registros se debe presionar la tecla LCTRL.",
         "Termina la ejecucion de la consola.",
         "Reproduce el himno de la alegria."};

int startConsole(){
    char printBuf[1024];            //This is used to print to screen in real time
    uint8_t commandBuffer[1024];       //This is used to prepare the next command
    uint16_t commandBufferPos = 0;
    char exit = 0;                  //Create console command for exit
   
    printText(LINE_INDICATOR);
    while (exit == 0){
		int readQty = scanf("%1024s",1, printBuf);
		if (readQty > 0){
            for (int i = 0; i < readQty; i++){  
                if ((commandBufferPos > 0) || (commandBufferPos == 0 && printBuf[i] != BACKSPACE) ){
                    putChar(printBuf[i]);
                    
                    if (printBuf[i] == NEWLINE){
                        exit = processCommand(commandBuffer, commandBufferPos);
                        printText(LINE_INDICATOR);
                        for (int arg_idx = 0; arg_idx < commandBufferPos; arg_idx++){ //Clear buffer
                            commandBuffer[arg_idx] = 0;
                        }
                        commandBufferPos = 0;
                    }else if( printBuf[i] == BACKSPACE){
                        commandBufferPos--;
                    }else{
                        commandBuffer[commandBufferPos++] = printBuf[i];        //Stores in command buffer
                    }
                }
            }
		}
	}
    return 1;
}

//Returns exit status
char processCommand(uint8_t *str, int length){
    char command[length];
    int i = 0;
    while(str[i] == ' '){
        i++;
    }
    int cmd_idx= 0;
    while (str[i] != ' ' && i < length){     
        command[cmd_idx++] = str[i];
        i++;
    } 
    command[cmd_idx] = 0;
     
    char arguments[length];
    uint8_t arg_idx = 0;
    uint8_t argQty = 0;
    while(i < length){
        if( str[i] != ' '){
            arguments[arg_idx++] = str[i];
        }else{
            argQty ++;
        }
        i++;
    }
    arguments[arg_idx] = 0;


    for (int i = 0; i < COMMAND_QTY; i++){
        if (strcmp(command,commandNames[i]) == 0){ 
            return (*commands[i])(argQty,arguments);
        }
    }
    printf("Comando no reconocido\n",0);
    return 0;
}

char setFontSize(uint8_t argQty, char arguments[argQty], uint8_t rewrite){
    if (argQty < 1){
        printf("Ingrese una fuente entre 1 y 4\n",0);
    }else if (argQty > 1){
        printf("Demasiados argumentos\n",0);
    }else{
        int num = strToNum(arguments, strlen(arguments));
        if (num > 4 || num < 1){
            printf("Ingrese una fuente entre 1 y 4\n",0);
        }else{
            _sys_setFontSize(num, rewrite);
        }
    }
    return 0;
}

char help(uint8_t argumentQty, char arguments[argumentQty]){
    if( argumentQty == 0 ) {
        printf("Los comandos disponibles son:\n",0);
        for (int i = 0; i < COMMAND_QTY; i++){
            printf("%s\n",1,commandNames[i]);
        }
    }else if (argumentQty > 1){
        printf("Demasiados argumentos para help\n",0);
    }else {
        if(strcmp(arguments, "please") == 0){
            printf("No.\n", 0);
            return 0;
        }else if (strcmp(arguments, "all")== 0){
            for (int i = 0; i < COMMAND_QTY; i++){
                printf("%s: %s\n\n",2,commandNames[i], commandDescriptions[i]);
            }
            return 0;
        }else {
            for (int i = 0; i < COMMAND_QTY; i++){
                if (strcmp(arguments, commandNames[i]) == 0){
                    printf("%s\n",1,commandDescriptions[i]);
                    return 0;
                }
            }
        }
        printf("Argumento invalido para help\n", 0);
    }
    return 0;
}

char clean(uint8_t argumentQty, char arguments[argumentQty]){
    if( argumentQty != 0 ){
        printf("Argumento invalido para clean\n", 0);
    }else{
        cleanScreen();
    }
    return 0;
}

char startTron(uint8_t argumentQty, char arguments[argumentQty]){
    if( argumentQty != 0){
        printf("Argumento invalido para tron\n", 0);
    }else{
        tron();
    }
    return 0;
}

char callExec(uint8_t argumentQty, char arguments[argumentQty]){
    if(argumentQty == 0){
        char *args[1] = {"processA"};
        execve(&processA, args);
        char *args2[1] = {"processB"};
        execve(&processB, args2);
    }else{
        printf("Argumento invalido para exec\n", 0);
    }
    return 0;
}

char callFree(uint8_t argumentQty, char arguments[argumentQty]){
    if(argumentQty == 1 && isHexaNumber(arguments)){
        char flag = 0;
        uint64_t ptr = hexaStrToNum(arguments, strlen(arguments), &flag);
        if (flag == 1){
            printerr("Numero muy grande. Overflow\n",0);
        }else{
            const freedBytes = free(ptr);
            printf("%x bytes liberados\n", 1, freedBytes);
        }       
    }else{
        printf("Argumento invalido para free\n", 0);
    }
    return 0;
}

char callMalloc(uint8_t argumentQty, char arguments[argumentQty]){
    if(argumentQty == 1 && isHexaNumber(arguments)){
        char flag = 0;
        uint64_t size = hexaStrToNum(arguments, strlen(arguments), &flag);
        if (flag == 1){
            printerr("Numero muy grande. Overflow\n",0);
        }else{
            uint64_t * ptr = malloc(size);
            if (ptr == NULL){
                printf("No se reservo memoria\n",0);
            }else {
                printf("Se reservo memoria en la direccion %x\n",1,ptr);
            }
        }       
    }else{
        printf("Argumento invalido para malloc\n", 0);
    }
    return 0;
}

char callMemoryDump(uint8_t argumentQty, char arguments[argumentQty]){
    if(argumentQty == 1 && isHexaNumber(arguments)){
        char flag = 0;
        uint64_t direction = hexaStrToNum(arguments, strlen(arguments), &flag);
        if (flag == 1){
            printerr("Direccion muy grande. Overflow\n",0);
        }else{
            memoryDump(direction);
        }       
    }else{
        printf("Argumento invalido para memory-dump\n", 0);
    }
    return 0;
}

char time(uint8_t argumentQty, char arguments[argumentQty]){
    if( argumentQty != 0){
        printf("Argumento invalido para time\n", 0);
    }else{
        displayTime();
    }
    return 0;
}

char callZeroDivision(uint8_t argumentQty, char arguments[argumentQty]){
    if( argumentQty != 0){
        printf("Argumento invalido para time\n", 0);
    }else{
        zeroDivision();
    }
    return 0;    
}

char callInvalidOpcode(uint8_t argumentQty, char arguments[argumentQty]){
    if( argumentQty != 0){
        printf("Argumento invalido para time\n", 0);
    }else{
        invalidOpcode();
    }
    return 0;
}

char callSetFontSize(uint8_t argumentQty, char arguments[argumentQty]){
    if( argumentQty != 1){
        printf("Argumento invalido para time\n", 0);
    }else{
        setFontSize(argumentQty, arguments, 1);
    }
    return 0;
}

char exitConsole(uint8_t argumentQty, char arguments[argumentQty]){
    if( argumentQty != 0){
        printf("Argumento invalido para time\n", 0);
    }else{
        return 1;
    }
    return 0;
}

char callInforeg(uint8_t argumentQty, char arguments[argumentQty]){
    if( argumentQty != 0){
        printf("Argumento invalido para time\n", 0);
    }else{
        printRegs();
    }
    return 0;
}

char callHimnoAlegria(uint8_t argumentQty, char arguments[argumentQty]){
    if( argumentQty != 0){
        printf("Argumento invalido para time\n", 0);
    }else{
        himnoAlegria();
    } 
    return 0;
}

char callPrintProcesses(uint8_t argumentQty, char arguments[argumentQty]){
    if( argumentQty != 0){
        printf("Argumento invalido para time\n", 0);
    }else{
        printProcesses();
    } 
    return 0;
}

