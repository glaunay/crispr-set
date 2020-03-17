//File: two_bits_encoding.h
#ifndef TWO_BITS_ENCODING_H
#define TWO_BITS_ENCODING_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>


#define TWO_BIT_MASK (3ULL)
#define BITS_PER_BYTE (8)
#define BIG_ENOUGH (1024)

typedef struct binaryWord {
    uint64_t value;
    size_t len;
} binaryWord_t;

/*
Much inspired from 
    https://stackoverflow.com/questions/39242932/how-to-encode-char-in-2-bits/39244735#39244735

A 65 01000|00|1  0
C 67 01000|01|1  1
G 71 01000|11|1  3
T 84 01010|10|0  2

*/

binaryWord_t encode(char *original/*, size_t *strLen*/);
void encode_bis(char *original, binaryWord_t *result);
void decode(binaryWord_t encoded, char *decoded, bool rna_flag);
binaryWord_t truncateBinaryLeft(binaryWord_t binaryWord, int lenTo);
void truncateBinaryLeft_bis(binaryWord_t binaryWord, int lenTo, binaryWord_t *truncBinaryWord);
bool IsBitSet(int b, int pos);
uint8_t hammingDistance(binaryWord_t w1, binaryWord_t w2);
void showbits( uint64_t x );

#endif
