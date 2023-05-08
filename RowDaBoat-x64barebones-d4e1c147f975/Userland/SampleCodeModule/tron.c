#include <tron.h>

#define UP_ARROW 300
#define DOWN_ARROW 301
#define LEFT_ARROW 302
#define RIGHT_ARROW 303
#define ESCAPE 27

#define EXIT    -1
#define EMPTY   0
#define DRAW    0
#define PLAYER1 1
#define PLAYER2 2
#define UP      0
#define DOWN    1
#define LEFT    2
#define RIGHT   3
#define COLOR1  0x0000FF
#define COLOR2  0xFF0000
#define COLOR3  0xEBE534
#define BORDER_COLOR        0x000066
#define BACKGROUND_COLOR    0x000040
#define LINE_WIDTH  4
#define SNAKE_WIDTH 10
#define CL 523
#define D 587
#define E 659
#define F 698
#define G 784
#define A 880
#define B 988
#define CH 1046

void manageDirections(uint16_t letter, int * dir,int lastDir,uint16_t leftSymbol,uint16_t upSymbol,uint16_t rightSymbol,uint16_t downSymbol);
int manageMovement(int dir, int *px,int *py,int gameWidth,int gameHeight);
void draw();
void winner(int player);

int game(uint16_t gamePixelWidth, uint16_t gamePixelHeight, uint16_t topLeftX, uint16_t topLeftY, int points1, int points2, uint8_t screnBpp,
                uint8_t rectSprite1[SNAKE_WIDTH][SNAKE_WIDTH*screnBpp/8],uint8_t rectSprite2[SNAKE_WIDTH][SNAKE_WIDTH*screnBpp/8], 
                uint8_t rectSprite3[SNAKE_WIDTH][SNAKE_WIDTH*screnBpp/8]);

void playSound(); 
void buildBackground24Bpp(uint16_t screenWidth, uint16_t screenHeight ,uint16_t gamePixelWidth, uint16_t gamePixelHeight, uint16_t topLeftX, uint16_t topLeftY, uint8_t screenBpp);
void printBackground(uint16_t screenWidth, uint16_t screenHeight);
void generateRect(uint16_t width, uint16_t height, uint32_t color,  uint8_t screenBpp, uint8_t location[height][width*screenBpp/8]);


extern uint8_t screenBackground[768][1024*3];
extern uint8_t *memmov(uint64_t destination, uint64_t source, uint64_t lenght);

void tron(){
    uint16_t width=_sys_getScreenWidth(),height=_sys_getScreenHeight();
    uint8_t bpp = _sys_getScreenBpp();
    uint16_t gamePixelWidth=800,gamePixelHeight=600;
    uint16_t topLeftX=width/2-gamePixelWidth/2;
	uint16_t topLeftY=height/2-gamePixelHeight/2;
    uint8_t previousFontSize = _sys_getFontSize();

    uint8_t rectSprite1[SNAKE_WIDTH][SNAKE_WIDTH*bpp];
    uint8_t rectSprite2[SNAKE_WIDTH][SNAKE_WIDTH*bpp];  
    uint8_t rectSprite3[SNAKE_WIDTH][SNAKE_WIDTH*bpp];
    
    cleanScreen();
    _sys_setFontSize(3,0);
    printf("Cargando ...\n",0);
    switch (bpp){       //If a different bpp is used, another function should be used
        case 24:
            buildBackground24Bpp(width, height, gamePixelWidth,gamePixelHeight,topLeftX,topLeftY, bpp);
            break;
    }
    generateRect(SNAKE_WIDTH,SNAKE_WIDTH,COLOR1,bpp,rectSprite1);
    generateRect(SNAKE_WIDTH,SNAKE_WIDTH,COLOR2,bpp,rectSprite2);
    generateRect(SNAKE_WIDTH,SNAKE_WIDTH,COLOR3,bpp,rectSprite3);

    printf("Juego listo, toque espacio para continuar\n",0);
    int points1=0,points2=0;
    int winner;
    int exit=0;
    uint16_t buffer[1];
    while (!exit){
        int readQty = scanf("%1S",1, buffer);
		if (readQty > 0){
            switch (buffer[0])
            {
            case ESCAPE:
                cleanScreen();
                exit=1;
                _sys_setFontSize(previousFontSize,1);
                break;
            case ' ':
                printBackground(width, height);
                formatPrint("Azul  %d : %d  Rojo", 0x9d46fa, 0, 13, 2, points1, points2);
                winner=game(gamePixelWidth,gamePixelHeight,topLeftX,topLeftY, points1, points2,bpp,rectSprite1,rectSprite2,rectSprite3);
                if(winner==EXIT){
                    cleanScreen();
                    exit=1;
                    _sys_setFontSize(previousFontSize,1);
                    break;
                }
                if(winner==PLAYER1){
                    points1++;
                }else if(winner==PLAYER2){
                    points2++;
                }
                _sys_setFontSize(3,0);
                formatPrint("Azul  %d : %d  Rojo", 0x9d46fa, 0, 13, 2, points1, points2);
                playSound();
                break;
            }
        }
    }
}

void generateRect(uint16_t width, uint16_t height, uint32_t color,  uint8_t screenBpp, uint8_t location[height][width*screenBpp/8]){
    uint8_t bytePerPixel = screenBpp / 8;
    for (int i = 0; i < width; i++){
        for (int j = 0; j < height; j++){
            for (int k = 0; k < bytePerPixel; k++){
                location[i][j*bytePerPixel+k]  = (color>>k*8) & 255;
            }
        }
    }
}

void rectToScreenBuffer(uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, uint32_t color, uint8_t screenBpp){
    uint8_t bytePerPixel = screenBpp / 8;
    for (int i = yPos; i < height + yPos; i++){
        for (int j = xPos; j < xPos + width; j++){
            for (int k = 0; k < bytePerPixel; k++){
                screenBackground[i][j*bytePerPixel+k] = color>>(k*8) & 255;
            }
        }
    }
}

void buildBackground24Bpp(uint16_t screenWidth, uint16_t screenHeight ,uint16_t gamePixelWidth, uint16_t gamePixelHeight, uint16_t topLeftX, uint16_t topLeftY, uint8_t screenBpp){
    rectToScreenBuffer(0,0,screenWidth,screenHeight,0,screenBpp);//BLACK
    rectToScreenBuffer(topLeftX-LINE_WIDTH, topLeftY-LINE_WIDTH, gamePixelWidth+LINE_WIDTH*2, gamePixelHeight+LINE_WIDTH*2, BORDER_COLOR,screenBpp); //Border
    rectToScreenBuffer(topLeftX,topLeftY, gamePixelWidth, gamePixelHeight, BACKGROUND_COLOR, screenBpp);   //Main Square
}

void printBackground(uint16_t screenWidth, uint16_t screenHeight){
    _sys_drawSprite(0,0,screenWidth,screenHeight,screenBackground);
}

int game(uint16_t gamePixelWidth, uint16_t gamePixelHeight, uint16_t topLeftX, uint16_t topLeftY, int points1, int points2, uint8_t screnBpp,
                uint8_t rectSprite1[SNAKE_WIDTH][SNAKE_WIDTH*screnBpp/8],uint8_t rectSprite2[SNAKE_WIDTH][SNAKE_WIDTH*screnBpp/8], 
                uint8_t rectSprite3[SNAKE_WIDTH][SNAKE_WIDTH*screnBpp/8]){

    int gameWidth=gamePixelWidth/SNAKE_WIDTH;
    int gameHeight=gamePixelHeight/SNAKE_WIDTH; 
    int posMatrix[gameHeight][gameWidth];
    int p1x=gameWidth/4;
    int p1y=gameHeight/2;
    int p2x=(gameWidth*3)/4;
    int p2y=gameHeight/2;
    int dir1=RIGHT;
    int dir2=LEFT;
    int timePerMov= 50;
    uint16_t buffer[1];  

    _sys_setFontSize(1,0);
    formatPrint("[ESC]: Salir",0xFFFFFF, 0,0,0);
    formatPrint("[Space]: Nuevo Juego",0xFFFFFF, 1,0,0);
    _sys_setFontSize(3,0);
    for (int i = 0; i < gameHeight; i++){
        for (int j = 0; j < gameWidth; j++){
            posMatrix[i][j]=EMPTY;
        }
    }
    posMatrix[p1y][p1x]=PLAYER1;
    posMatrix[p2y][p2x]=PLAYER2;


    _sys_drawSprite(topLeftX+p1x*SNAKE_WIDTH,topLeftY+p1y*SNAKE_WIDTH,SNAKE_WIDTH,SNAKE_WIDTH,rectSprite1);
    _sys_drawSprite(topLeftX+p2x*SNAKE_WIDTH,topLeftY+p2y*SNAKE_WIDTH,SNAKE_WIDTH,SNAKE_WIDTH,rectSprite2);
  
    int hitWall1=0,hitWall2=0;
    int player1Lost=0,player2Lost=0;    
    uint32_t millis = _sys_getMillis();
    uint32_t ant=millis;
    int lastDir1=RIGHT,lastDir2=LEFT;
    while (!player1Lost && !player2Lost){
        int readQty = scanf("%1S",1, buffer);
        if (readQty > 0){
            if(buffer[0]== ESCAPE){
                return -1;
            }
            manageDirections(buffer[0],&dir1, lastDir1, 'a','w','d','s');                
            manageDirections(buffer[0],&dir2, lastDir2, LEFT_ARROW,UP_ARROW,RIGHT_ARROW,DOWN_ARROW);
        }    
        if(millis>ant+timePerMov){
                
            hitWall1=manageMovement(dir1,&p1x,&p1y,gameWidth,gameHeight);
            hitWall2=manageMovement(dir2,&p2x,&p2y,gameWidth,gameHeight);
            player1Lost=hitWall1|| posMatrix[p1y][p1x]!=EMPTY;
            player2Lost=hitWall2 || posMatrix[p2y][p2x]!=EMPTY;
            if(player1Lost && player2Lost){
               draw();
            }else if(player1Lost){
                winner(2);                   
            }else if(player2Lost){
                winner(1);                   
            }else{
                posMatrix[p1y][p1x]=PLAYER1;
                posMatrix[p2y][p2x]=PLAYER2; 
                if(p1x==p2x && p1y == p2y){
                    draw();
                    player1Lost=1;
                    player2Lost=1;
                    _sys_drawSprite(topLeftX+p1x*SNAKE_WIDTH,topLeftY+p1y*SNAKE_WIDTH,SNAKE_WIDTH,SNAKE_WIDTH,rectSprite3);
                }else{
                    _sys_drawSprite(topLeftX+p1x*SNAKE_WIDTH,topLeftY+p1y*SNAKE_WIDTH,SNAKE_WIDTH,SNAKE_WIDTH,rectSprite1);
                    _sys_drawSprite(topLeftX+p2x*SNAKE_WIDTH,topLeftY+p2y*SNAKE_WIDTH,SNAKE_WIDTH,SNAKE_WIDTH,rectSprite2);
                }
                if(timePerMov>2){
                    timePerMov=timePerMov*0.95;
                }
                lastDir1=dir1;
                lastDir2=dir2;
            }
            ant=millis;
        }
        millis=_sys_getMillis();
    }
    if(player1Lost && player2Lost){
        return DRAW;
    }
    if(player2Lost){
        return PLAYER1;
    }
    return PLAYER2;
}

int manageMovement(int dir, int *px,int *py,int gameWidth,int gameHeight){
    switch (dir){
        case RIGHT:
            (*px)++;
            if((*px)>=gameWidth){
                return 1;
            }
            break;
        case LEFT:
            (*px)--;
            if((*px)<0){
                return 1;
            }
            break;
        case UP:
            (*py)--;
            if((*py)<0){
                return 1;
            }
            break;
        case DOWN:
            (*py)++;
            if((*py)>=gameHeight){
                return 1;
            }
            break;
    }
    return 0;
}


void manageDirections(uint16_t letter, int * dir,int lastDir,uint16_t leftSymbol,uint16_t upSymbol,uint16_t rightSymbol,uint16_t downSymbol){
    if(letter==leftSymbol){
        if(lastDir!=RIGHT)
                (*dir)=LEFT;
    }else if(letter==upSymbol){
        if(lastDir!=DOWN)
                (*dir)=UP;
    }else if(letter==rightSymbol){
        if(lastDir!=LEFT)
                (*dir)=RIGHT;
    }else if(letter==downSymbol){
        if(lastDir!=UP)
                (*dir)=DOWN;
    }
}

void draw(){
    _sys_setFontSize(3,0);
    formatPrint("Empate", 0xEB8C34, 15, 19, 0);
}

void winner(int player){
    _sys_setFontSize(3,0);
    formatPrint("%s gana", 0xEB8C34, 15, 17, 1, player==PLAYER1?"Azul":"Rojo");
}

void playSound(){
    uint32_t startTime;
	uint16_t delta = 200;
	startTime = _sys_getMillis();
	_sys_beep(F);
	while (_sys_getMillis() < startTime + delta){}
	startTime = _sys_getMillis();
	_sys_beep(E);
	while (_sys_getMillis() < startTime + delta){}
	startTime = _sys_getMillis();
	_sys_beep(D);
	while (_sys_getMillis() < startTime + delta){}
	startTime = _sys_getMillis();
	_sys_beep(CL);
	while (_sys_getMillis() < startTime + delta){}
	startTime = _sys_getMillis();
	_sys_beep(D);
	while (_sys_getMillis() < startTime + delta){}
	_sys_beep(0);
}