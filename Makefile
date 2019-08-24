CC=gcc
CFLAGS := -std=c99 -pthread -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-function

UDIS86=build/udis86/decode.o\
		build/udis86/itab.o\
		build/udis86/syn-att.o\
		build/udis86/syn-intel.o\
		build/udis86/syn.o\
		build/udis86/udis86.o\

HEADERS=src/common.h\
		src/debugger.h\
		src/decompiler.h\
		src/elfparser.h\
		src/memory.h\
		src/ptracer.h

OBJ=build/decompiler.o\
	build/elfparser.o\
	build/memory.o\
	build/ptracer.o

all: $(OBJ) $(HEADERS)
	@ printf "%8s %-40s %s\n" $(CC) "debugger"
	@ gcc $(OBJ) $(UDIS86) src/main.c -o debugger $(CFLAGS)

build/%.o: src/%.c $(HEADERS)
	@ printf "%8s %-40s %s\n" $(CC) $@
	@ gcc -c $< -o $@ $(CFLAGS)

clean:
	rm -v build/*.o
