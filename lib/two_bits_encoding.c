#include "two_bits_encoding.h"
#include <assert.h>



/*
Much inspired from 
    https://stackoverflow.com/questions/39242932/how-to-encode-char-in-2-bits/39244735#39244735

A 65 01000|00|1  0
C 67 01000|01|1  1
G 71 01000|11|1  3
T 84 01010|10|0  2

*/
int *createZeroIntArray(size_t size) {
    int *array = malloc( size * sizeof(int) );
    for (size_t i = 0; i < size ; i++) {
        array[ i ] = 0;
    }
    return array;
}

binaryWord_t encode(char *original/*, size_t *strLen*/) {
   // printf("Encoding %s\n", original);
    //size_t length = strlen(original);
    binaryWord_t b;
    b.value = 0;
    b.len = strlen(original);
    assert(b.len * 2 <= sizeof(uint64_t) * BITS_PER_BYTE);
    b.value = 0;
   
    for (size_t i = 0; i < b.len; i++) {
        b.value = (b.value << 2) | ((original[i] >> 1) & TWO_BIT_MASK);
    }
    return b;
}

// Here for python module compatibility
void encode_bis(char *original, binaryWord_t *result) {
   // printf("Encoding %s\n", original);
    //size_t length = strlen(original);

    result->value = 0;

    result->len = strlen(original);
    assert(result->len * 2 <= sizeof(uint64_t) * BITS_PER_BYTE);

    for (size_t i = 0; i < result->len; i++) {
        
        result->value = ((result->value) << 2) | ((original[i] >> 1) & TWO_BIT_MASK);
       // printf("BIS [%d] %c,%d == %llu\n", i, original[i], original[i], *result);
    }
   
    //return result;
}

void decode(binaryWord_t encoded, char *decoded, bool rna_flag) {

    //int i = sizeof(uint64_t) * BITS_PER_BYTE / 2;

    int i = encoded.len;
    for (decoded[i--] = '\0'; i >= 0; i--, encoded.value >>= 2) {

        unsigned char byte = encoded.value & TWO_BIT_MASK;

        if (byte == 2) {
            byte = (rna_flag) ? 'U' : 'T';
        } else {
            byte = 'A' | (byte << 1);
        }

        decoded[i] = byte;
    }
}

// Setting bits to zero on the left side of the bit array
binaryWord_t truncateBinaryLeft(binaryWord_t binaryWord, int lenTo) {
    binaryWord_t result;

    int lenFrom = binaryWord.len;
    assert(lenTo < lenFrom);
    int offset = 64 - (lenFrom * 2); // left side of the array unused during encoding
    offset += (lenFrom - lenTo) * 2; // left side of the array we want to drop
#ifdef DEBUG
    showbits(binaryWord.value);
    printf("Offset is [%d] + %d \n", 64 - (lenFrom * 2), (lenFrom - lenTo) * 2);
#endif
    result.value = binaryWord.value << offset;
#ifdef DEBUG
    showbits(result.value);
    printf("Shifting right by %d\n",offset);
#endif
    result.value = result.value >> offset;
#ifdef DEBUG    
    showbits(result.value);
#endif
    return result;
}

// Setting bits to zero on the left side of the bit array
void truncateBinaryLeft_bis(binaryWord_t binaryWord, int lenTo, binaryWord_t *truncBinaryWord) {
    int lenFrom = binaryWord.len;
    assert(lenTo < lenFrom);
    truncBinaryWord->len = lenTo;
    int offset = 64 - (lenFrom * 2); // left side of the array unused during encoding
    offset += (lenFrom - lenTo) * 2; // left side of the array we want to drop
#ifdef DEBUG
    showbits(binaryWord.value);
    printf("Offset is [%d] + %d \n", 64 - (lenFrom * 2), (lenFrom - lenTo) * 2);
#endif
    truncBinaryWord->value = binaryWord.value << offset;
#ifdef DEBUG
    showbits(binaryWord.value);
    printf("Shifting right by %d\n",offset);
#endif
    truncBinaryWord->value = truncBinaryWord->value >> offset;
#ifdef DEBUG    
    showbits(binaryWord.value);
#endif

}

void showbits( uint64_t x )
{
    for (int i = (sizeof(uint64_t) * 8) - 1; i >= 0; i--)
    {
       putchar(x & (1ull << i) ? '1' : '0');
    }
    printf("\n");
}

bool IsBitSet(int b, int pos)
{
   return (b & (1 << pos)) != 0;
}

uint8_t hammingDistance(binaryWord_t w1, binaryWord_t w2) {
    uint64_t tmp;
    uint8_t dist = 0;
    assert (w1.len == w2.len);
    for (size_t i = 0 ; i < w1.len ; i++) {
        tmp = w1.value ^ w2.value;
        //printf(">>%llu<<\n", tmp);
        tmp = (tmp >> i * 2) & TWO_BIT_MASK;
        //printf("%zu:%llu\n", i, tmp);
        dist += tmp != 0; // SLOW ?? Mask it to value 1 or 0 may be better?
        //dist += __builtin_popcount(tmp);
    }
    return dist;
}
