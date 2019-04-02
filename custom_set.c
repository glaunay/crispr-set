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
    if(x == 0 && list[x].value == value)
        found = true;
    #ifdef DEBUG        
        fprintf(stderr,"%llu == %llu\n", list[hi_x].value, value);
    #endif
    
    /*
    if(found)
        fprintf(stderr, "Found it!\n");
    fprintf(stderr,"[[%d ((up:%d,lo:%d))]] %llu == %llu\n", listLength, hi_x, lo_x, list[hi_x].value, value);
    if(!found) {
        if (list[hi_x].value == value)
            return hi_x;
        if (list[lo_x].value == value)
            return lo_x;
    }
    */        
    return found ? x : -1;    
}

void freeSetData(integerSet_t *set) {
    int i;
    uint64_t *tmp_ptr;
    if(set->data == NULL)
        return;
   for (i = 0 ; i < set->_size; i++) {
        if(set->data[i].primeList != NULL) {
            tmp_ptr = set->data[i].primeList;
            free(tmp_ptr);
            set->data[i].primeList = NULL;
            //free(set->data->primeList);
        }
    }
    free(set->data);
    set->data = NULL;
}

integerSet_t *destroySet(integerSet_t *set) {
    freeSetData(set);
    free(set);
    return NULL;
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

// DANGEROUS FUNCTION FREEING TARGET FIELD ON THE FLY !!
void copyWordInto(int64word_t *sourceOne, int64word_t *sourceTwo, int64word_t *target) {

#ifdef DEBUG
    fprintf(stderr, "Copying following words\n");
    wordPrint(sourceOne, stderr);
    if (sourceTwo != NULL)
        wordPrint(sourceTwo, stderr);
#endif
    int iCount = sourceOne->count;
    int jCount = sourceTwo != NULL ? sourceTwo->count : 0;
    int iNbPrime = sourceOne->nbPrime;
    int jNbPrime = sourceTwo != NULL ? sourceTwo->nbPrime : 0;


    target->count = iCount + jCount;
    target->value = sourceOne->value;
    target->nbPrime = iNbPrime + jNbPrime;
   
    // In case we deal with words featuring prime lists
    uint64_t *buffer;

    if (target->nbPrime > 0) {
        //Concatenate source and target prime lists
        size_t sz = (target->nbPrime * sizeof(uint64_t));
        buffer = malloc(sz);
        if(iNbPrime > 0)
            memcpy(buffer                   , sourceOne->primeList , iNbPrime * sizeof(uint64_t) );
        if(jNbPrime > 0)
            memcpy(&(buffer[iNbPrime]), sourceTwo->primeList, jNbPrime * sizeof(uint64_t) );
        
        if (target->primeList != NULL) {
            fprintf(stderr, "Allocation error\n");
            exit(1);
        }

        target->primeList = malloc(sz);
        memcpy(target->primeList, buffer, sz);

        free(buffer);
    }
   
   #ifdef DEBUG
        fprintf(stderr, "Build Word is \n");
        wordPrint(target, stderr);
    #endif
}

// Susbtract FROM the iSet the elements also found in jSet
integerSet_t *setSubstract(integerSet_t *iSet, integerSet_t *jSet) {
    int i, j;
    integerSet_t *subSet = newSetZeros(iSet->size);
   /* integerSet_t *subSet = malloc(sizeof(integerSet_t));
    subSet->data = malloc( iSet->size * sizeof(int64word_t) );
    subSet->size = 0;
    */
    for (i = 0; i < iSet->size ; i++) {
        j = dichotomicSearch(jSet->data, jSet->size, iSet->data[i].value);
        if (j == -1 ) {
           /* subSet->data[subSet->size].value = iSet->data[i].value;
            subSet->data[subSet->size].count = iSet->data[i].count;            */
            copyWordInto( &(iSet->data[i]), NULL, &(subSet->data[subSet->size]) );
            subSet->size++;
        } else {
            #ifdef DEBUG            
                fprintf(stderr, "FOUND %lld\n", iSet->data[i].value);
            #endif
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
            copyWordInto( &(iSet->data[i]), &(jSet->data[j]), &(interSet->data[interSet->size]) );
            interSet->size++;
            #ifdef DEBUG
                printf("FOUND\n");
            #endif
        }
    }

#ifdef DEBUG
        printf("Size of intersect Set: %d\n", interSet->size);
#endif


    return interSet;
}

void wordPrint(int64word_t *word, FILE *stream) {
    int j;
    fprintf(stream,"value:%llu\tcount:%d, primeList:[", word->value, word->count);
    for (j = 0; j < word->nbPrime; j++)
        fprintf(stderr,"%llu ", word->primeList[j]);
    fprintf(stderr, "]  \n");
}
void setPrint(integerSet_t *set, FILE *stream ) {
    int x, y;
    fprintf(stream, "# %d items set\n", set->size);

    for (x = 0; x < set->size; x++) {
        fprintf(stream, "%llu:%d", set->data[x].value, set->data[x].count);
        if(set->data[x].nbPrime > 0) {
            /* Print prime list*/
            fprintf(stream, "[");
            for (y = 0; y < set->data[x].nbPrime ; y++) {
                fprintf(stream, "%llu", set->data[x].primeList[y]);
                if(y < set->data[x].nbPrime - 1)
                    fprintf(stream, ",");
            }
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
#ifdef DEBUG
    fprintf(stderr, "Performing zero allocation of size %d\n", newSetSize);
#endif

    integerSet_t *newSet = malloc(sizeof(integerSet_t));
    int i;
    newSet->size = 0;
    newSet->_size = newSetSize;
    newSet->data = malloc(newSet->_size * sizeof(int64word_t));
    for (i = 0; i < newSet->_size ; i++) {
        newSet->data[i].nbPrime = 0;
        newSet->data[i].primeList = NULL;
        newSet->data[i].count = 0;
        newSet->data[i].value = 0;
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

/* Uniq primeList elements */
/*  Shortening size list using nbPrime, allocation is not retailored !!! */
void tidySet(integerSet_t *oriSet) {
    int iWord, iPrime;
    uint64_t *primeBuffer = NULL;
    int64word_t *cWord;
    uint64_t prevPrime, currPrime;
    int count;
    for (iWord = 0; iWord < oriSet->size; iWord++) {
        cWord = &(oriSet->data[iWord]);
        sortWordPrimeList(cWord);
        primeBuffer = malloc(cWord->nbPrime * sizeof(uint64_t));
        
        prevPrime = cWord->primeList[0];
        primeBuffer[0] = prevPrime;
        count = 1;

        for (iPrime = 1 ; iPrime < cWord->nbPrime; iPrime++) {
            currPrime = cWord->primeList[iPrime];
            if(currPrime == prevPrime)
                continue;
            primeBuffer[count] = currPrime;
            prevPrime = currPrime;
            count++;
        }
        memcpy(cWord->primeList, primeBuffer,  count * sizeof(uint64_t));
        cWord->nbPrime = count;
        free(primeBuffer);
    }
}


void sortWordPrimeList(int64word_t *word) {
    qsort (word->primeList, word->nbPrime, sizeof(uint64_t), uint64_compare);
}

int uint64_compare (const void * a, const void * b)
{
    uint64_t* i0 = (uint64_t *) a;
    uint64_t* i1 = (uint64_t *) b;
    
    if (i0 < i1) return -1;
    if (i0 > i1) return 1;
    return 0;
}