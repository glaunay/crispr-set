//File: custom_transform.h
#ifndef CUSTOM_TRANSFORM_H
#define CUSTOM_TRANSFORM_H

#include <stdio.h>
#include <stdint.h>
#include "custom_set.h";


integerSet_t *project(integerSet_t *fromSet, int froDim, int targetDim, int base);
int compareByValue (const void * a, const void * b);
void rankSetByValue(integerSet_t *set);

#endif