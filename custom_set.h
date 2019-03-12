#include <stdio.h>
#include <stdint.h>

typedef struct {
    int size;
    uint64_t *data;
} integerSet_t;



integerSet_t *setIntersect(integerSet_t *xSet, integerSet_t *ySet);
void setPrint(integerSet_t *set);
integerSet_t *destroySet(integerSet_t *set);
integerSet_t *newSetFromFile(char *filePath);
void moveSet(integerSet_t *sourceSet, integerSet_t *targetSet );
integerSet_t *setSubstract(integerSet_t *iSet, integerSet_t *jSet);
int constructFilePath(char *fileLocation, char *includedFileList, char **filePath);