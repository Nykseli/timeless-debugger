CFLAGS := -std=c99 -pthread -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-function

UDIS86=build/udis86/decode.o\
		build/udis86/itab.o\
		build/udis86/syn-att.o\
		build/udis86/syn-intel.o\
		build/udis86/syn.o\
		build/udis86/udis86.o\

OBJ=build/decompiler.o\
	build/elfparser.o\
	build/memory.o\
	build/ptracer.o

all: $(OBJ)
	gcc -g $(OBJ) $(UDIS86) src/main.c -o debugger $(CFLAGS)

build/%.o: src/%.c
	gcc -g -c $< -o $@ $(CFLAGS)

# build/decompiler.o: src/decompiler.c
# 	gcc -c src/decompiler.c -o build/decompiler.o
#
# build/elfparser.o: src/elfparser.c
# 	gcc -c src/elfparser.c -o build/elfparser.o
#


tracery: ptracery.c
	gcc $(UDIS86) ptracery.c
