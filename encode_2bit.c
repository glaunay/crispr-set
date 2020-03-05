#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <getopt.h>

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

uint64_t encode(char *original, size_t *strLen) {
   // printf("Encoding %s\n", original);
    //size_t length = strlen(original);
    *strLen = strlen(original);
    assert(*strLen * 2 <= sizeof(uint64_t) * BITS_PER_BYTE);

    uint64_t result = 0;

    for (size_t i = 0; i < *strLen; i++) {
        result = (result << 2) | ((original[i] >> 1) & TWO_BIT_MASK);
    }

    return result;
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

void compareOneToFile(char *queryString, char *targetFile){
    FILE *fp = fopen(targetFile, "r");

    size_t strLenQuery;
    uint64_t binaryQuery = encode(queryString, &strLenQuery);

    size_t strLenCurr;
    uint64_t binaryCurr; 
    int bufferLength = 255;
    int d;
    char currString[bufferLength];
    int *distValues = malloc(sizeof(int) * strLenQuery + 1); 
    for ( size_t i = 0 ; i <= strLenQuery ; i++){
        distValues[i] = 0;
    }
    while(fgets(currString, bufferLength, fp)) {
        currString[strcspn(currString, "\n")] = 0;
        binaryCurr = encode(currString, &strLenCurr);
        assert(strLenCurr == strLenCurr);
        d = hammingDistance(binaryQuery, binaryCurr, strLenQuery);
        distValues[d]++;
        //printf("########\nD=%d\n%s\n%s\n", d, queryString, currString);
    }

    fclose(fp);
    for ( size_t i = 0 ; i <= strLenQuery ; i++){
        printf("%zu:%d\n", i, distValues[i]);
    } 
    free(distValues);
}


int main(int argc, char *argv[]) {
    int c;
    const char    *short_opt = "a:b:t:h";
    struct option   long_opt[] =
    {
        {"help",                 no_argument, NULL, 'h'},
        {"sequenceA",            required_argument, NULL, 'a'},
        {"sequenceB",            required_argument, NULL, 'b'},
        {"FileForsequencesB",            required_argument, NULL, 't'}
    };
    
    char *stringToEncodeQuery = NULL;
    char *stringToEncodeTarget = NULL;
    char *targetFile = NULL;

    while((c = getopt_long(argc, argv, short_opt, long_opt, NULL)) != -1) {
        switch(c) {
            case -1:       /* no more arguments */
            case 0:        /* long options toggles */
            break;
            case 'a':                
                stringToEncodeQuery = strdup(optarg);
                break;
            case 'b':                
                stringToEncodeTarget = strdup(optarg);
                break;
            case 't':                
                targetFile = strdup(optarg);
                break;
   
        }
    }
    if (targetFile != NULL && stringToEncodeQuery !=  NULL) {       
        compareOneToFile(stringToEncodeQuery, targetFile);
        return(1);
    }



    //char *segment = "GCCGTGCTAAGCGTAACAACTTCAAATCCGCG";

    printf("Trying to encode \"%s\"\n", stringToEncodeQuery);
    size_t strLenQuery;
    uint64_t binaryQuery = encode(stringToEncodeQuery, &strLenQuery);

    printf("Encoded string of size %lu\n", strLenQuery);
    printf("%llu\n", binaryQuery);
    char string[BIG_ENOUGH];
    decode(binaryQuery, string, false, strLenQuery);
    printf("%s\n", string);

    if (stringToEncodeTarget != NULL) {
        printf("Trying to encode \"%s\"\n", stringToEncodeTarget);
        size_t strLenTarget;
        uint64_t binaryTarget = encode(stringToEncodeTarget, &strLenTarget);
        assert(strLenTarget == strLenQuery);
        //printf("Encoded string of size %lu\n", strLenQuery);
        printf("%llu\n", binaryTarget);
        //char string[BIG_ENOUGH];
        decode(binaryTarget, string, false, strLenTarget);
        printf("%s\n", string);

        int d = hammingDistance(binaryQuery, binaryTarget, strLenQuery);

        printf("Counted %d missmatches\n", d);
    }

    return 0;
}