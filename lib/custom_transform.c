#define _GNU_SOURCE

#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "custom_set.h"
#include "custom_transform.h"
#include "two_bits_encoding.h"

integerSet_t *project(integerSet_t *fromSet, int fromDim, int targetDim, int base,
                        uint64_t (*truncFn)(uint64_t, int, int, int) ){
    #ifdef DEBUG
        fprintf(stderr,"PROJECTING...\n");
        setPrint(fromSet, stderr);
    #endif
    integerSet_t *targetSet = newSetZeros(fromSet->size);
    #ifdef DEBUG
        fprintf(stderr,"targetSet content\n");
        setPrint(targetSet, stderr);
    #endif
    int i;
    #ifdef DEBUG
        fprintf(stderr,"Encoding fromSet...(%d loops)\n", fromSet->size);
    #endif
    for (i = 0 ; i < fromSet->size ; i++) {
        #ifdef DEBUG
            fprintf(stderr, "fromSet index %d\t%lld:%d\n", i, fromSet->data[i].value, fromSet->data[i].count);
        #endif
        targetSet->data[i].count = fromSet->data[i].count;
        //targetSet->data[i].value = customTruncate(fromSet->data[i].value, fromDim, targetDim, base);
        targetSet->data[i].value = truncFn(fromSet->data[i].value, fromDim, targetDim, base);
        targetSet->data[i].nbPrime = 1;
        targetSet->data[i].primeList = malloc(sizeof(uint64_t));
        targetSet->data[i].primeList[0] = fromSet->data[i].value;
        targetSet->size++;
        #ifdef DEBUG
            fprintf(stderr, "targetSet index %d\n", i);
            wordPrint(&(targetSet->data[i]), stderr );
        #endif
        //fprintf(stderr, "targetSet index %d\t%lld:%d\n", i, fromSet->data[i].value, fromSet->data[i].count);
    }
#ifdef DEBUG
    fprintf(stderr,"Ranking this\n");
    setPrint(targetSet, stderr);
#endif    
    rankSetByValue(targetSet);
#ifdef DEBUG
    fprintf(stderr,"into this\n");
    setPrint(targetSet, stderr);
    fprintf(stderr,"Compressing above set\n");
#endif
    // Now Compress it as finalSet
    
    uint64_t buffSize = pow(base, fromDim - targetDim);
    uint64_t *buffer = malloc(sizeof(uint64_t) * buffSize);

    integerSet_t *finalSet = newSetZeros(fromSet->size);
    //finalSet->_size = finalSet->size;  // Store the real size to allow proper free
   
    
    int64word_t *newWord = &(finalSet->data[0]);
    newWord->value = targetSet->data[0].value;
    newWord->count = targetSet->data[0].count;
    buffSize = 1;
    buffer[0] = targetSet->data[0].primeList[0];

    int64word_t *currWord;
    for(i = 1; i < targetSet->size ; i++) {
        currWord = &(targetSet->data[i]);
#ifdef DEBUG
        fprintf(stderr, "current Word is\t");
        wordPrint(currWord, stderr);
#endif
        // Current word and new word collides, we store the   
        if (currWord->value == newWord->value) {
            newWord->count += currWord->count;
            buffer[buffSize] = currWord->primeList[0] ;//i;
            buffSize++;
            continue;
        }
        //Flush&Store new word
        newWord->primeList = malloc(buffSize * sizeof(uint64_t));
        memcpy(newWord->primeList, buffer, sizeof(uint64_t) * buffSize);
        newWord->nbPrime = buffSize;
        finalSet->size++;
        // Position newWord on current
        newWord = &(finalSet->data[finalSet->size]);
        buffSize = 1;
        buffer[0] = currWord->primeList[0];
        newWord->count = currWord->count;
        newWord->value += currWord->value;
    }
    #ifdef DEBUG
        fprintf(stderr, "Trailing buffer of size %d:\n", buffSize);
        for ( i = 0; i < buffSize; i++)
            fprintf(stderr, ">%lld<", buffer[i]);
        fprintf(stderr, "\n");
    #endif
    if(newWord->primeList != NULL) {
        fprintf(stderr, "Alloc Error\n");
        exit(1);
    }
    //Store trailing new word
    newWord->primeList = malloc(buffSize * sizeof(uint64_t));
    memcpy(newWord->primeList, buffer, sizeof(uint64_t) * buffSize);
    newWord->nbPrime = buffSize;
    finalSet->size++;

    free(buffer);
    destroySet(targetSet);

    #ifdef DEBUG
        fprintf(stderr,"into this\n");
        setPrint(finalSet, stderr);
    #endif

    return finalSet;
}

// Mimic customTruncate interface to work w/ *fn 
uint64_t truncateBinaryLeftWrapper(uint64_t value, int sizeFrom, int sizeTo, int _) {
    binaryWord_t w1;
    w1.value = value;
    w1.len = sizeFrom;

    binaryWord_t w2 = truncateBinaryLeft(w1, sizeTo);
    
    return w2.value;
}

uint64_t customTruncate(uint64_t value, int sizeFrom, int sizeTo, int base) {
    uint64_t _value = value;
    uint64_t offset = 0;
    uint64_t w;
    int i;
    uint64_t cur;
#ifdef DEBUG
    fprintf(stderr,"Projecting value %lld from %dD to %dD\n", value, sizeFrom, sizeTo);
#endif
    for(i = sizeFrom ; i >= sizeTo ; i--) {
        cur = int64_exp((uint64_t) base, (uint64_t)i);
        w       = value / cur; // Floor division
        offset += w     * cur;
        value   = value % cur;
    }
#ifdef DEBUG
    fprintf(stderr,"\t=> %lld - %lld = %lld\n", _value, offset, _value - offset);
#endif
    return _value - offset;
}

uint64_t int64_exp( uint64_t base, uint64_t exponent) {
    int i;
    uint64_t value = 1;
    for (i = 0; i < exponent; i++)
        value *= base;
    return value;
}
int compareByValue (const void * a, const void * b)
{
    int64word_t* w0 = (int64word_t *) a;
    int64word_t* w1 = (int64word_t *) b;
    
    if (w0->value < w1->value) return -1;
    if (w0->value > w1->value) return 1;
    return 0;
}

void rankSetByValue(integerSet_t *set) {
    qsort (set->data, set->size, sizeof(int64word_t), compareByValue);
}
