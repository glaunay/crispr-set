#define _GNU_SOURCE

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "custom_set.h"
#include "custom_transform.h"

int dichotomicSearch(int64word_t *list, int listLength, uint64_t value) {
    int x = listLength / 2;
    int lo_x = 0;
    int hi_x = listLength;
    //int _x = listLength; 
    bool found = false;
   
#ifdef DEBUG
    printf("Total list size %d\n", listLength);
    printf("Starting index x is %d\n", x);
#endif

    while (hi_x - lo_x > 1) {
#ifdef DEBUG
        printf("Looking at %llu[%d]\n", list[x].value, x);
#endif        
        if (list[x].value == value) {
#ifdef DEBUG
            printf("Found it at pos %d!!\n", x);
#endif
            found = true;
            break;
        }
        if (x == 0)
            break;
/*
#ifdef DEBUG        
        if(list[x] < value) {
            printf("GOING UP\n");
        } else {
            printf("GOING DOWN\n");
        }
#endif
*/
        //printf("%d erased %d\n", x, _x);
        if (value < list[x].value) { // current value if bigger than seeked one 
#ifdef DEBUG
            printf("GOING DOWN\n");
#endif
            hi_x = x;
        } else { // current value if lower than seeked one 
#ifdef DEBUG            
            printf("GOING UP\n");
#endif
            lo_x = x;
        }
#ifdef DEBUG        
        printf("x = %d + %d / 2\n", hi_x, lo_x);
#endif
        x = (hi_x + lo_x) / 2;
#ifdef DEBUG        
        printf("Next x is %d\n\n", x);
#endif
    }

    if (list[hi_x].value == value)
        return hi_x;
    if (list[lo_x].value == value)
        return lo_x;
        
    return found ? x : -1;    
}

void freeSetData(integerSet_t *set) {
    int i;
    if(set->data == NULL)
    return;
    int nbData = set->_size > 0 ? set->_size : set->size;

    for (i=0 ; i < nbData; i++)
        if(set->data->primeList != NULL)
            free(set->data->primeList);
    free(set->data);
}

integerSet_t *destroySet(integerSet_t *set) {
    fprintf(stderr,"CLICK\n");
    freeSetData(set);
    free(set);
    return set;
}



// Move source content into target, free source

void moveSet(integerSet_t *sourceSet, integerSet_t *targetSet ) {
    targetSet->size = sourceSet->size;
    targetSet->_size = sourceSet->_size;
    freeSetData(targetSet);
    
    targetSet->data = sourceSet->data;
    
    sourceSet->data = NULL;
    free(sourceSet);
}

void copyWord(int64word_t *source, int64word_t *target) {
    target->count = source->count;
    target->value = source->value;
    // In case we deal with words featuring prime lists
    int *buffer;
    if (source->nbPrime > 0 || target->nbPrime > 0) {
        //Concatenate source and target prime lists
        size_t sz = (source->nbPrime + target->nbPrime) * sizeof(uint64_t);
        buffer = malloc(sz);
        if(source->nbPrime > 0)
            memcpy(buffer, source->primeList, source->nbPrime * sizeof(uint64_t) );
        if(target->nbPrime > 0)
            memcpy(&(buffer[source->nbPrime]), target->primeList, target->nbPrime * sizeof(uint64_t) );
        if (target->primeList != NULL) 
            free(target->primeList);
        target->primeList = malloc(sz);
        // Copy buffer content
        memcpy(target->primeList, buffer, sz);
    }

    target->nbPrime += source->nbPrime;    
}

// Susbtract FROM the iSet the elements also found in jSet
integerSet_t *setSubstract(integerSet_t *iSet, integerSet_t *jSet) {
    int i;
    integerSet_t *subSet = newSetZeros(iSet->size);
   /* integerSet_t *subSet = malloc(sizeof(integerSet_t));
    subSet->data = malloc( iSet->size * sizeof(int64word_t) );
    subSet->size = 0;
    */
    for (i = 0; i < iSet->size ; i++) {
#ifdef DEBUG
        printf("LF: %llu\n", iSet->data[i].value);
#endif
        if (dichotomicSearch(jSet->data, jSet->size, iSet->data[i].value) == -1 ) {
           /* subSet->data[subSet->size].value = iSet->data[i].value;
            subSet->data[subSet->size].count = iSet->data[i].count;            */
            copyWord( &(iSet->data[i]), &(subSet->data[subSet->size]) );
            subSet->size++;
        }

    }

    return subSet;
}

// We do pre allocation on smallest
integerSet_t *setIntersect(integerSet_t *xSet, integerSet_t *ySet) {
    int i,j;
    integerSet_t *iSet = xSet->size < ySet->size ? xSet : ySet ;
    integerSet_t *jSet = xSet->size < ySet->size ? ySet : xSet ;
    /*
    integerSet_t *interSet = malloc(sizeof(integerSet_t));
    interSet->data = malloc( iSet->size * sizeof(int64word_t) );
    interSet->size = 0;*/
    integerSet_t *interSet = newSetZeros(iSet->size);  
    uint64_t seekedValue;

    for (i = 0; i < iSet->size ; i++) {
        seekedValue = iSet->data[i].value;
#ifdef DEBUG
        printf("LF: %llu\n", seekedValue);
#endif
        j = dichotomicSearch(jSet->data, jSet->size, seekedValue);
        if ( j > -1 ) {
          /*  interSet->data[interSet->size].value = iSet->data[i].value;
            interSet->data[interSet->size].count = iSet->data[i].count + jSet->data[j].count;*/
            copyWord( &(iSet->data[i]), &(interSet->data[interSet->size]) );
            interSet->size++;
        }
    }

#ifdef DEBUG
        printf("Size of intersect Set: %d\n", interSet->size);
#endif


    return interSet;
}

void setPrint(integerSet_t *set, FILE *stream ) {
    int x, y;
    fprintf(stream, "# %d items set\n", set->size);

    for (x = 0; x < set->size; x++) {
        fprintf(stream, "%llu:%d", set->data[x].value, set->data[x].count);
        if(set->data[x].nbPrime > 0) {
            /* Print prime list*/
            fprintf(stream, "[");
            for (y = 0; y < set->data[x].nbPrime ; y++)
                fprintf(stream, "%llu", set->data[x].primeList[y]);
            fprintf(stream, "]\n");
        } else {
            if (x < set->size - 1)
                fprintf(stream, ",");
        }
    }
    fprintf(stream, "\n");
}

// Initialize a interSet for a max of provided number of words
// Return a structure with the *size* field equal to zero, max size is stored in *_size*
integerSet_t *newSetZeros(int newSetSize) {
    integerSet_t *newSet = malloc(sizeof(integerSet_t));
    int i;
    newSet->size = 0;
    newSet->_size = newSetSize;
    newSet->data = malloc(newSet->size * sizeof(int64word_t));
    for (i = 0; i < newSet->size ; i++) {
        newSet->data[i].nbPrime = 0;
        newSet->data[i].primeList = NULL;
    }
    return newSet;
}

integerSet_t *newSetFromFile(char *filePath) {
    FILE *fp = NULL;
    char * line = NULL;
    size_t len = 0;
    size_t read;

    integerSet_t *newSet = NULL;
    //; = malloc(sizeof(integerSet_t));
    //newSet->size = -1;
    //newSet->_size = 0;
    fp = fopen(filePath, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error, can't open%s\n", filePath);
        exit(1);
    }
    uint64_t cValue;
    int count;
    int total = -1;
    while ((read = getline(&line, &len, fp)) != -1) {
        //printf("Retrieved line of length %zu:\n", read);
        if(total < 0) {
            sscanf (line, "%d", &total);
            newSet = newSetZeros(total);
            continue;
        }

        sscanf (line, "%llu %d", &cValue, &count);
        newSet->data[newSet->size].value = cValue;
        newSet->data[newSet->size].count = count;
        newSet->size += 1;        
    }
    fclose(fp);
    if (line)
        free(line);
   
    if (newSet->size != total) {
        fprintf(stderr, "Uncorrect number of elements on input\n");
        exit(1);
    }
    return newSet;
}

