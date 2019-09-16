CC=gcc
CFLAGS := -std=c99 -pthread -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-function

REST_API=build/c-rest-api/restapi.o

UDIS86=build/udis86/decode.o\
		build/udis86/itab.o\
		build/udis86/syn-att.o\
		build/udis86/syn-intel.o\
		build/udis86/syn.o\
		build/udis86/udis86.o

HEADERS=src/common.h\
		src/database.h\
		src/debugger.h\
		src/decompiler.h\
		src/elfparser.h\
		src/memory.h\
		src/ptracer.h\
		src/options.h

OBJ=build/database.o\
	build/decompiler.o\
	build/elfparser.o\
	build/memory.o\
	build/ptracer.o\
	build/restapi.o

all: $(OBJ) $(HEADERS)
	@ printf "%8s %-40s %s\n" $(CC) "debugger"
	@ gcc $(OBJ) $(REST_API) $(UDIS86) src/main.c -o debugger $(CFLAGS)

build/%.o: src/%.c $(HEADERS)
	@ printf "%8s %-40s %s\n" $(CC) $@
	@ gcc -c $< -o $@ $(CFLAGS)

clean:
	rm -v build/*.o
