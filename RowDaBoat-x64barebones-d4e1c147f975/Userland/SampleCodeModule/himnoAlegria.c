#include <himnoAlegria.h>

#define CL 523
#define D 587
#define E 659
#define F 698
#define G 784
#define A 880
#define B 988
#define CH 1046

void himnoAlegria(){
    uint32_t startTime;
	uint16_t delta = 50;

	startTime = _sys_getMillis();
    int partSize=15;
    int notes [2][15] = {
        { E,E,F,G,G,F,E,D,CL,CL,D,E,E,D,D},
        { E,E,F,G,G,F,E,D,CL,CL,D,E,D,CL,CL}    
    };
    
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < partSize; j++)
        {
        _sys_beep(notes[i][j]);
        while (_sys_getMillis() < startTime + delta){}
	    startTime = _sys_getMillis();
         _sys_beep(0);
        while (_sys_getMillis() < startTime + delta){}
	    startTime = _sys_getMillis();
        }
        _sys_beep(0);
        while (_sys_getMillis() < startTime + delta){}
	    startTime = _sys_getMillis();
    }
}