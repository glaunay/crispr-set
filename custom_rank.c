#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "custom_set.h"
#include "custom_rank.h"

int compare (const void * a, const void * b)
{
    int64word_t* w0 = (int64word_t *) a;
    int64word_t* w1 = (int64word_t *) b;
    
    return w1->count - w0->count;
}


void rankSetQ(integerSet_t *set) {
    qsort (set->data, set->size, sizeof(int64word_t), compare);
}


rankCell_t *rankSet(integerSet_t *set) {
    rankCell_t *rankedList = NULL;
    int i;
    for (i = 0; i < set->size; i++) {
        rankedList = addCell(rankedList, &set->data[i]);
    //printRankedList(rankedList);
    }
    return rankedList;
}

void printRankedList(rankCell_t *root, FILE *stream) {
    rankCell_t *currCell = root;
    while(currCell->next != NULL) {
        fprintf(stream, "%llu:%d,", currCell->elem->value, currCell->elem->count);
        currCell = currCell->next;
    }
    fprintf(stream, "%llu:%d\n", currCell->elem->value, currCell->elem->count);
}
/* insert a newly created rank element in chain*/
rankCell_t *addCell(rankCell_t *root, int64word_t *elem) {

    rankCell_t *newCell = malloc ( sizeof(rankCell_t) );
    newCell->elem = elem;
    newCell->before = NULL;
    newCell->next = NULL;
        
    //fprintf(stderr, "Adding (%lld:%d)\n", elem->value, elem->count);

    if(root == NULL)
        return newCell;
    
    
    rankCell_t *currentCell = root; 
    while(currentCell->next != NULL) {
        if (newCell->elem->count > currentCell->elem->count)
            break;
        currentCell = currentCell->next;
    }

    if (currentCell->before == NULL && currentCell->next == NULL) { // Current cell is 1st position and alone in chain
            if (newCell->elem->count > currentCell->elem->count) { // place ahead
                newCell->next = currentCell;
                currentCell->before = newCell;
                root = newCell;
            } else { // place behind
                newCell->before = currentCell;
                currentCell->next = newCell;
            }
    } else if(currentCell->before == NULL) { // Current cell is 1st position and not alone in chain
        newCell->next = currentCell;
        currentCell->before = newCell;
        root = newCell;
    } else if(currentCell->next == NULL) { //Current cell is last in chain, we re sagfe to append to it
            newCell->before = currentCell;
            currentCell->next = newCell;
    } else {// we insert at n-1, anywhere inside
        currentCell->before->next = newCell;
        newCell->before = currentCell->before;
        currentCell->before = newCell;
        newCell->next = currentCell;
    }
    return root;
}
