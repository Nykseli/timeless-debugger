

UDIS86=build/udis86/decode.o\
		build/udis86/itab.o\
		build/udis86/syn-att.o\
		build/udis86/syn-intel.o\
		build/udis86/syn.o\
		build/udis86/udis86.o\

OBJ=build/decompiler.o

all: $(OBJ)
	gcc $(OBJ) $(UDIS86) src/elfparser.c -o debugger

build/decompiler.o: src/decompiler.c
	gcc -c src/decompiler.c -o build/decompiler.o
