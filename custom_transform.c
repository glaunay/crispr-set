#define _GNU_SOURCE

#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "custom_set.h"
#include "custom_transform.h"

integerSet_t *project(integerSet_t *fromSet, int fromDim, int targetDim, int base) {
    fprintf(stderr,"PROJECTING...\n");
    setPrint(fromSet, stderr);
    fprintf(stderr,">%lld<\n", fromSet->data[1].value);
    fprintf(stderr,"targetSet <- newSetZeros\n");
    integerSet_t *targetSet = newSetZeros(fromSet->size);
    fprintf(stderr,"targetSet content\n");
    setPrint(targetSet, stderr);
    fprintf(stderr,"targetSet hidden size>>%d<<\n", targetSet->_size);
    fprintf(stderr, "Trying to access to elet pos 1 in fromSet\n");
    fprintf(stderr,">%lld<\n", fromSet->data[1].value);
    fprintf(stderr,"fromSet content\n");
    setPrint(fromSet, stderr);

    int i;
    fprintf(stderr,"AA...\n");
    for (i = 0 ; i < fromSet->size ; i++) {
        fprintf(stderr, "(%d)uu:%lld\n", i, fromSet->data[i].value);
        targetSet->data[i].count = fromSet->data[i].count;
        targetSet->data[i].value = customTruncate(fromSet->data[i].value, fromDim, targetDim, base);
        targetSet->data[i].nbPrime = 1;
        targetSet->data[i].primeList = malloc(sizeof(uint64_t));
        targetSet->data[i].primeList[0] = fromSet->data[i].value;
        targetSet->size++;
    }
    fprintf(stderr,"BB...\n");
    rankSetByValue(targetSet);

    // Now Compress it as finalSet

    int buffSize = pow(base, fromDim - targetDim);
    uint64_t *buffer = malloc(sizeof(uint64_t) * buffSize);

    integerSet_t *finalSet = newSetZeros(fromSet->size);
    //finalSet->_size = finalSet->size;  // Store the real size to allow proper free
   
    
    int64word_t *newWord = &(finalSet->data[0]);
    newWord->value = targetSet->data[0].value;
    newWord->count = targetSet->data[0].count;
    buffSize = 1;
    buffer[0] = targetSet->data[0].primeList[0];
    finalSet->size = 1;

    int64word_t *currWord;
    for(i = 0; i < targetSet->size ; i++) {
        currWord = &(targetSet->data[i]);
        // Current word and new word collides, we store the   
        if (currWord->value == newWord->value) {
            newWord->count += currWord->count;
            buffSize++;
            buffer[buffSize] = currWord->primeList[0] ;//i;
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
    //Store trailing new word
    newWord->primeList = malloc(buffSize * sizeof(uint64_t));
    memcpy(newWord->primeList, buffer, sizeof(uint64_t) * buffSize);
    newWord->nbPrime = buffSize;
    finalSet->size++;

    destroySet(targetSet);
    return finalSet;
}

int64_t customTruncate(int64_t value, int sizeFrom, int sizeTo, int base) {
    int64_t _value = value;
    int64_t offset = 0;
    int64_t w;
    int i;
    fprintf(stderr,"[%lld] from %d to %d\n", value, sizeFrom, sizeTo);
    for(i = sizeFrom ; i <= sizeTo ; i--) {
        w       = value / (int64_t)( pow(base, i) ); // Floor division
        offset += w     * (int64_t)( pow(base, i));
        value   = value % (int64_t)( pow(base, i));
    }
    fprintf(stderr,"\t=> %lld - %lld = %lld\n", _value, offset, _value - offset);
    return _value - offset;
}
    
int compareByValue (const void * a, const void * b)
{
    int64word_t* w0 = (int64word_t *) a;
    int64word_t* w1 = (int64word_t *) b;
    
    return w1->value - w0->value;
}


void rankSetByValue(integerSet_t *set) {
    qsort (set->data, set->size, sizeof(int64word_t), compareByValue);
}
