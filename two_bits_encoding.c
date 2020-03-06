#include "two_bits_encoding.h"




/*
Much inspired from 
    https://stackoverflow.com/questions/39242932/how-to-encode-char-in-2-bits/39244735#39244735

A 65 01000|00|1  0
C 67 01000|01|1  1
G 71 01000|11|1  3
T 84 01010|10|0  2

*/

uint64_t encode(char *original, size_t *strLen) {
   // printf("Encoding %s\n", original);
    //size_t length = strlen(original);
    *strLen = strlen(original);
    assert(*strLen * 2 <= sizeof(uint64_t) * BITS_PER_BYTE);

    uint64_t result = 0;

    for (size_t i = 0; i < *strLen; i++) {
        
        result = (result << 2) | ((original[i] >> 1) & TWO_BIT_MASK);
       // printf("[%d] %c,%d == %llu\n", i, original[i], original[i], result);
    }
    //printf("%llu\n", result);
    return result;
}

void encode_bis(char *original, size_t *strLen, uint64_t *result) {
   // printf("Encoding %s\n", original);
    //size_t length = strlen(original);

    *result = 0;

    *strLen = strlen(original);
    assert(*strLen * 2 <= sizeof(uint64_t) * BITS_PER_BYTE);

    for (size_t i = 0; i < *strLen; i++) {
        
        *result = ((*result) << 2) | ((original[i] >> 1) & TWO_BIT_MASK);
       // printf("BIS [%d] %c,%d == %llu\n", i, original[i], original[i], *result);
    }
   
    //return result;
}

void decode(uint64_t encoded, char *decoded, bool rna_flag, size_t strLen) {

    int i = sizeof(uint64_t) * BITS_PER_BYTE / 2;

    i = strLen;
    for (decoded[i--] = '\0'; i >= 0; i--, encoded >>= 2) {

        unsigned char byte = encoded & TWO_BIT_MASK;

        if (byte == 2) {
            byte = (rna_flag) ? 'U' : 'T';
        } else {
            byte = 'A' | (byte << 1);
        }

        decoded[i] = byte;
    }
}

bool IsBitSet(int b, int pos)
{
   return (b & (1 << pos)) != 0;
}


uint8_t hammingDistance(uint64_t w1, uint64_t w2, size_t strLen) {
    uint64_t tmp;
    uint8_t dist = 0;
    for (size_t i = 0 ; i < strLen ; i++) {
        tmp = w1 ^ w2;
        //printf(">>%llu<<\n", tmp);
        tmp = (tmp >> i * 2) & TWO_BIT_MASK;
        //printf("%zu:%llu\n", i, tmp);
        dist += tmp != 0; // SLOW ?? Mask it to value 1 or 0 may be better?
        //dist += __builtin_popcount(tmp);
    }
    return dist;
}
