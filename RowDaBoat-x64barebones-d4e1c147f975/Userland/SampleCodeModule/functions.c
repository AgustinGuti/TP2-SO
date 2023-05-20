#include <functions.h>

int strcmp(const char* str1, const char* str2) {
	for (; *str1 == *str2 && *str1 != '\0'; str1++, str2++);
	return *str1 - *str2;
}

void hexToStr(char * char_num, long num){
	  int digits = hexNumLength(num);
    if(num < 0){
      num*=-1;
    }

    for(int i=digits-1 ; i>=0 ; --i){
      char aux = num%16;
      if (aux < 10){
          char_num[i] = aux + '0';
      }else{
          char_num[i] = aux - 10 + 'A';
      }
      num/=16;
    }
	  char_num[digits] = 0;
}

void decToStr(char * char_num, int num){
	  int digits = decNumLength(num);
    if(num < 0){
      num*=-1;
    }

    for(int i=digits-1 ; i>=0 ; --i){
      char_num[i] = num%10 + '0';
      num/=10;
    }
	  char_num[digits] = 0;
}

int hexNumLength(long num){
    if(num < 0){
      num *= -1;
    }
    int length = 0;
    while(num != 0){
        length++;
        num/=16;
    }
	if (length == 0)
		length = 1;
  
  return length;
}

int decNumLength(int num){
    if(num < 0){
      num *= -1;
    }
    int length = 0;
    while(num != 0){
        length++;
        num/=10;
    }
	if (length == 0)
		length = 1;
    return length;
}

/***
 * Funcion que transforma un string a un entero.
 *
 * Argumentos:
 * - char * str: Puntero al string que se convertira en un entero.
 * - int longitud: Longitud del string.
 *
 * Retorno:
 * - Transformacion del string a un int.
 ***/

int strToNum(char * str, int longitud){
    int neg = 0;

    if(str[0] == '-'){
      neg = 1;
    }

    int num = 0;

    for(int i=neg ; i<longitud ; i++){
      num = num*10 + str[i] - '0';
    }

    if(neg){
      num*=-1;
    }

    return num;
}

uint64_t hexaStrToNum(char * str, int longitud, char *overflowFlag){
    uint64_t num = 0;
    int digit;
    for(int i=0 ; i<longitud ; i++){
      if(str[i]>='0' && str[i]<='9'){
        digit=str[i]-'0';
      } 
      if(str[i]>='a' && str[i]<='f'){
        digit=10+str[i]-'a';
      } 
      if(str[i]>='A' && str[i]<='F'){
        digit=10+str[i]-'A';
      }
      if (num*16 + digit < num){  //Overflow
        *overflowFlag = 1;
      }
      num = num*16 + digit;
      
    }
    return num;
}


uint32_t strlen(char *str){
    uint32_t len = 0;
    while (str[len] != 0){
      len++;
    }
    return len;
}

void buildRectSprite(uint16_t width, uint16_t height, uint32_t color, uint8_t **location){
  for (int i = 0; i < height; i++){
    for (int j = 0; j < width; j++){
      *(*location) = 150;
      location ++;
    }
  }
}

int isHexaNumber(char * str){
  int i=0;
  while (str[i]!=0){
    if((str[i]<'0'|| str[i]>'9') && (str[i]<'A' || str[i]>'F') && (str[i]<'a' || str[i]>'f')){
      return 0;
    }  
    i++;
  }
  return 1;
  
  
}