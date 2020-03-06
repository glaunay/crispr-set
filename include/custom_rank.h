//File: custom_rank.h
#ifndef CUSTOM_RANK_H
#define CUSTOM_RANK_H

#include <stdio.h>
#include <stdint.h>
#include "custom_set.h"

typedef struct rankCell {
    int64word_t *elem;
    struct rankCell *before;
    struct rankCell *next;
} rankCell_t;

rankCell_t *addCell(rankCell_t *root, int64word_t *elem);
rankCell_t *rankSet(integerSet_t *set);
void printRankedList(rankCell_t *rankedList, FILE *stream);
void rankSetQ(integerSet_t *set);
#endif