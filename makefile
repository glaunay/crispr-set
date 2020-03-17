# GL basic makefile
# Add debug and valgrind compiler option

GCC_FLAGS = -pedantic -lm -std=c99 -Wall -L./lib -I./include
DEBUG=-g -D DEBUG

all: setCompare encode kmerHamming

debug: GCC_FLAGS += $(DEBUG)
debug: kmerHamming#all

setCompare: lib/main_setCompare.c
	gcc -o bin/$@ $< $(GCC_FLAGS) lib/custom_*.c lib/two_bits_encoding.c lib/io_utils.c

encode: lib/main_2bits.c
	gcc -o bin/$@ $< $(GCC_FLAGS) lib/custom_*.c lib/two_bits_encoding.c

kmerHamming: lib/main_kmissmatch.c
	gcc -o bin/$@ $< $(GCC_FLAGS) lib/io_utils.c lib/two_bits_encoding.c

#hello.o: hello.c
#	 gcc -c hello.c
     
clean: 
	rm ./bin/*