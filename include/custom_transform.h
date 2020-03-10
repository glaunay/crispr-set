//File: custom_transform.h
#ifndef CUSTOM_TRANSFORM_H
#define CUSTOM_TRANSFORM_H

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "custom_set.h"


integerSet_t *project(integerSet_t *fromSet, int fromDim, int targetDim, int base,
                        uint64_t (*truncFn)(uint64_t, int, int, int) );
int compareByValue (const void * a, const void * b);
void rankSetByValue(integerSet_t *set);
uint64_t customTruncate(uint64_t value, int sizeFrom, int sizeTo, int base);
uint64_t int64_exp( uint64_t base, uint64_t exponent);
uint64_t truncateBinaryLeftWrapper(uint64_t value, int sizeFrom, int sizeTo, int _);
#endif
