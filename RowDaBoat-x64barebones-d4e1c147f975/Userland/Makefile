include Makefile.inc

SAMPLE_DATA=0001-sampleDataModule.bin

all: sampleCodeModule sampleDataModule
buddy: sampleCodeModule2 sampleDataModule

sampleCodeModule:
	cd SampleCodeModule; make all

sampleCodeModule2:
	cd SampleCodeModule; make buddy

sampleDataModule:
	printf "This is sample data." >> $(SAMPLE_DATA) && dd if=/dev/zero bs=1 count=1 >> $(SAMPLE_DATA)

clean:
	cd SampleCodeModule; make clean
	rm -rf *.bin


.PHONY: sampleCodeModule all clean
