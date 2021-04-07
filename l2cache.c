#include<stdio.h>
#include<stdlib.h>
#include "hardware.h"
#include "l2cache.h"

int fetchL2Cache (long pa, struct Hardware *hardware) {
    int setIndex = (pa >> 5) & (0b1111111);
    int tag = (pa >> 12) & (0b1111111111111);

    for(int i=0;i<8;i++) {
        if(hardware->l2->sets[setIndex].valid[i] && hardware->l2->sets[setIndex].tags[i] == tag)
            return 0; // hit
    }

    return 1; //miss
}

void updateL2Cache(long pa, struct Hardware *hardware) {
    int setIndex = (pa >> 5) & (0b1111111);
    int tag = (pa >> 12) & (0b1111111111111);
    
    if(hardware->l2->sets[setIndex].listCount <= 8) { // when the set is not full
        int freeIndex;
        for(int i=0;i<8;i++) { // finding the free index 
            if(!hardware->l2->sets[setIndex].valid[i]){
                freeIndex=i;
                break;
            }
        }
        hardware->l2->sets[setIndex].valid[freeIndex] = 1;
        hardware->l2->sets[setIndex].tags[freeIndex] = tag;
        
        if(hardware->l2->sets[setIndex].listCount == 0){
            hardware->l2->sets[setIndex].head->index = freeIndex; // freeIndex will be 0 only
        }
        else {
            struct Node *node = (struct Node*) malloc(sizeof(struct Node));
            node->index = freeIndex;
            node->next = NULL;
            hardware->l2->sets[setIndex].tail->next = node;
            hardware->l2->sets[setIndex].tail = node;
        }

        hardware->l2->sets[setIndex].listCount++;
    }
    else { // when the set is full we use FIFO replacement policy
        struct Node *node = hardware->l2->sets[setIndex].head;
        hardware->l2->sets[setIndex].tags[node->index] = tag;
        hardware->l2->sets[setIndex].head = hardware->l2->sets[setIndex].head->next;
        hardware->l2->sets[setIndex].tail->next = node;
        hardware->l2->sets[setIndex].tail = node; 
    }
    
}