#define _GNU_SOURCE

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "custom_set.h";
#include "custom_transform.h";

integerSet_t *project(integerSet_t *fromSet, int fromDim, int targetDim, int base) {
    integerSet_t *targetSet = newSetZeros(fromSet->size);
    int i;
    for (i = 0 ; i < fromSet->size ; i++) {
        targetSet->data[i].count = fromSet->data[i].count;
        targetSet->data[i].value = truncate(fromSet->data[i].value, fromDim, targetDim, base);
    }
    rankSetByValue(targetSet);

    // Now Compress it as finalSet

    int buffSize = pow(base, fromDim - targetDim);
    int *buffer = malloc(sizeof(int) * buffSize);

    integerSet_t *finalSet = newSetZeros(fromSet->size);
    finalSet->_size = finalSet->size;  // Store the real size to allow proper free
   
    
    int64word_t *newWord = &(finalSet->data[0]);
    newWord->value = targetSet->data[0].value;
    newWord->count = targetSet->data[0].count;
    buffSize = 1;
    buffer[0] = 0;
    finalSet->size = 1;

    int64word_t *currWord;
    for(i = 0; i < targetSet->size ; i++) {
        currWord = &(targetSet->data[i]);

        if (currWord->value == newWord->value) {
            newWord->count += currWord->count;
            buffSize++;
            buffer[buffSize] = i;
            continue;
        }
        //Flush&Store new word
        newWord->primeList = malloc(buffSize * sizeof(int));
        memcpy(newWord->primeList, buffer, sizeof(int) * buffSize);
        newWord->nbPrime = buffSize;
        finalSet->size++;
        // Position newWord on current
        newWord = &(finalSet->data[finalSet->size]);
        buffSize = 1;
        buffer[0] = currWord->value;
        newWord->count = currWord->count;
        newWord->value += currWord->value;
    }
    //Store trailing new word
    newWord->primeList = malloc(buffSize * sizeof(int));
    memcpy(newWord->primeList, buffer, sizeof(int) * buffSize);
    newWord->nbPrime = buffSize;
    finalSet->size++;

    destroySet(targetSet);
    return finalSet;
}

int64_t truncate(int64_t value, int sizeFrom, int sizeTo, int base) {
    int64_t _value = value;
    int64_t offset = 0;
    int64_t w;
    int i;
    
    for(i = sizeFrom ; i <= sizeTo ; i--) {
        w       = value / int64_t( pow(base, i) ); // Floor division
        offset += w     * int64_t( pow(base, i));
        value   = value % int64_t( pow(base, i));
    }

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
