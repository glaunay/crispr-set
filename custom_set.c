#define _GNU_SOURCE

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "custom_set.h"


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


integerSet_t *destroySet(integerSet_t *set) {
    free(set->data);
    free(set);
    return set;
}



// Move source content into target, free source

void moveSet(integerSet_t *sourceSet, integerSet_t *targetSet ) {
    targetSet->size = sourceSet->size;
    if (targetSet->data != NULL)
        free(targetSet->data);
    targetSet->data = sourceSet->data;
    
    sourceSet->data = NULL;
    free(sourceSet);
}

// Susbtract FROM the iSet the elements also found in jSet
integerSet_t *setSubstract(integerSet_t *iSet, integerSet_t *jSet) {
    int i;

    integerSet_t *subSet = malloc(sizeof(integerSet_t));
    subSet->data = malloc( iSet->size * sizeof(int64word_t) );
    subSet->size = 0;
    
    for (i = 0; i < iSet->size ; i++) {
#ifdef DEBUG
        printf("LF: %llu\n", iSet->data[i].value);
#endif
        if (dichotomicSearch(jSet->data, jSet->size, iSet->data[i].value) == -1 ) {
            subSet->data[subSet->size].value = iSet->data[i].value;
            subSet->data[subSet->size].count = iSet->data[i].count;            
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

    integerSet_t *interSet = malloc(sizeof(integerSet_t));
    interSet->data = malloc( iSet->size * sizeof(int64word_t) );
    interSet->size = 0;
    uint64_t seekedValue;

    for (i = 0; i < iSet->size ; i++) {
        seekedValue = iSet->data[i].value;
#ifdef DEBUG
        printf("LF: %llu\n", seekedValue);
#endif
        j = dichotomicSearch(jSet->data, jSet->size, seekedValue);
        if ( j > -1 ) {
            interSet->data[interSet->size].value = iSet->data[i].value;
            interSet->data[interSet->size].count = iSet->data[i].count + jSet->data[j].count;
            interSet->size++;
        }
    }

#ifdef DEBUG
        printf("Size of intersect Set: %d\n", interSet->size);
#endif

    return interSet;
}

void setPrint(integerSet_t *set, FILE *stream ) {
    int x;
    
    fprintf(stream, "# %d items set\n", set->size);

    for (x = 0; x < set->size; x++) {
        fprintf(stream, "%llu:%d", set->data[x].value, set->data[x].count);
        if (x < set->size - 1)
            fprintf(stream, ",");
    }
    fprintf(stream, "\n");
}


integerSet_t *newSetFromFile(char *filePath) {
    FILE *fp = NULL;
    char * line = NULL;
    size_t len = 0;
    size_t read;

    integerSet_t *newSet = malloc(sizeof(integerSet_t));
    newSet->size = -1;

    fp = fopen(filePath, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error, can't open%s\n", filePath);
        exit(1);
    }
    uint64_t cValue;
    int count;
    int dataIndex = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        //printf("Retrieved line of length %zu:\n", read);
        sscanf (line, "%llu %d", &cValue, &count);
        if(newSet->size < 0) {
            newSet->size = cValue;
            newSet->data = malloc(newSet->size * sizeof(int64word_t));
            continue;
        }
        newSet->data[dataIndex].value = cValue;
        newSet->data[dataIndex].count = count;
        dataIndex += 1;        
    }
    fclose(fp);
    if (line)
        free(line);
   
    return newSet;
}

