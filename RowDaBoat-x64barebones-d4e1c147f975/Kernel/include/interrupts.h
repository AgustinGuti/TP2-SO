#ifndef _INTERRUPS_H_
#define _INTERRUPS_H_

#include <idtLoader.h>
#include <stdint.h>

void _irq00Handler(void);
void _irq01Handler(void);
void _irq02Handler(void);
void _irq03Handler(void);
void _irq04Handler(void);
void _irq05Handler(void);
void _sysCallHandler(void);

void _exception0Handler(void);
void _exception1Handler(void);
void _exception2Handler(void);
void _exception3Handler(void);
void _exception4Handler(void);
void _exception5Handler(void);
void _exception6Handler(void);
void _exception7Handler(void);
void _exception8Handler(void);
void _exception9Handler(void);
void _exception10Handler(void);
void _exception11Handler(void);
void _exception12Handler(void);
void _exception13Handler(void);
void _exception14Handler(void);
void _exception15Handler(void);
void _exception16Handler(void);
void _exception17Handler(void);
void _exception18Handler(void);
void _exception19Handler(void);
void _exception20Handler(void);

void _cli(void);

void _sti(void);

void _hlt(void);

void picMasterMask(uint8_t mask);

void picSlaveMask(uint8_t mask);

//Ends the execution of the cpu
void haltcpu(void);

#endif /* INTERRUPS_H_ */
