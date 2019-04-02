//File: custom_set.h
#ifndef CUSTOM_SET_H
#define CUSTOM_SET_H
#include <stdio.h>
#include <stdint.h>


typedef struct {
    int count;
    uint64_t value;
    int nbPrime;
    uint64_t *primeList;
    //uint64_t *primeValues;
} int64word_t;

typedef struct {
    int size;
    int _size;
    int64word_t *data;
} integerSet_t;


integerSet_t *newSetZeros(int newSetSize);
integerSet_t *setIntersect(integerSet_t *xSet, integerSet_t *ySet);
void setPrint(integerSet_t *set, FILE *stream);
void wordPrint(int64word_t *word, FILE *stream);
integerSet_t *destroySet(integerSet_t *set);
integerSet_t *newSetFromFile(char *filePath);
void moveSet(integerSet_t *sourceSet, integerSet_t *targetSet );
integerSet_t *setSubstract(integerSet_t *iSet, integerSet_t *jSet);
int constructFilePath(char *fileLocation, char *includedFileList, char *fileExtention, char **filePath);
int dichotomicSearch(int64word_t *list, int listLength, uint64_t value);
void rankSetQ(integerSet_t *set);
void copyWordInto(int64word_t *sourceOne, int64word_t *sourceTwo, int64word_t *target);
void sortWordPrimeList(int64word_t *word);
int uint64_compare (const void * a, const void * b);
void tidySet(integerSet_t *oriSet);

#endif