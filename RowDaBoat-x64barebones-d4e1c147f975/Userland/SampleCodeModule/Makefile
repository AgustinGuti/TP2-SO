include ../Makefile.inc

MODULE=0000-sampleCodeModule.bin
SOURCES=$(wildcard [^_]*.c)
SOURCES_ASM=$(wildcard asm/*.asm)
OBJECTS_ASM=$(SOURCES_ASM:asm/%.asm=obj/%.asm.o)


buddy: MEMFLAG=-DBUDDY

buddy: $(SOURCES) $(OBJECTS_ASM)
	$(GCC) $(GCCFLAGS) $(MEMFLAG)  -I./include -T sampleCodeModule.ld _loader.c $(OBJECTS_ASM) $(SOURCES) -o ../$(MODULE)
	$(GCC) $(GCCFLAGS) $(MEMFLAG)  -I./include -T sampleCodeModule.ld -Wl,--oformat=elf64-x86-64 _loader.c $(OBJECTS_ASM) $(SOURCES)  -o ../0000-sampleCodeModule.elf
	
all: MEMFLAG=-DDEFAULT

all:  $(SOURCES) $(OBJECTS_ASM)
	$(GCC) $(GCCFLAGS) $(MEMFLAG)  -I./include -T sampleCodeModule.ld _loader.c $(OBJECTS_ASM) $(SOURCES) -o ../$(MODULE)
	$(GCC) $(GCCFLAGS) $(MEMFLAG) -I./include -T sampleCodeModule.ld -Wl,--oformat=elf64-x86-64 _loader.c $(OBJECTS_ASM) $(SOURCES)  -o ../0000-sampleCodeModule.elf


obj/%.asm.o : asm/%.asm
	mkdir -p obj
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf *.o
	rm -rf obj

.PHONY: all clean print

