#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>


#define TWO_BIT_MASK (3)
#define BITS_PER_BYTE (8)
#define BIG_ENOUGH (1024)



/*
Much inspired from 
    https://stackoverflow.com/questions/39242932/how-to-encode-char-in-2-bits/39244735#39244735

A 65 01000|00|1  0
C 67 01000|01|1  1
G 71 01000|11|1  3
T 84 01010|10|0  2

*/

uint64_t encode(char *original, size_t *strLen);
void encode_bis(char *original, size_t *strLen, uint64_t *result);
void decode(uint64_t encoded, char *decoded, bool rna_flag, size_t strLen);
bool IsBitSet(int b, int pos);
uint8_t hammingDistance(uint64_t w1, uint64_t w2, size_t strLen);
