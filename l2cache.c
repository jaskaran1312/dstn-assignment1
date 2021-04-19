#include<stdio.h>
#include<stdlib.h>
#include "hardware.h"
#include "l2cache.h"
#include "l1cache.h"

int fetchL2Cache (int64_t pa, struct Hardware *hardware) {
    printf("Inside fetchL2Cache\n");
    fflush(stdout);
    int64_t setIndex = (pa >> 5) & (0x7f);
    int64_t tag = (pa >> 12) & (0x1fff);

    for(int i=0;i<8;i++) {
        if(hardware->l2->sets[setIndex].valid[i] && hardware->l2->sets[setIndex].tags[i] == tag) {
            printf("L2 hit\n");
            fflush(stdout);
            return 0; // hit
        }
            
    }
    printf("L2 Miss\n");
    fflush(stdout);
    return 1; //miss
}

int updateL2Cache(int64_t pa, struct Hardware *hardware) {
    printf("Inside updateL2Cache\n");
    fflush(stdout);
    int64_t setIndex = (pa >> 5) & (0x7f);
    int64_t tag = (pa >> 12) & (0x1fff);
    int dirty = 0;
    
    printf("SetIndex :%ld, tag: %ld\n",setIndex, tag);
    fflush(stdout);
    if(hardware->l2->sets[setIndex].listCount < 8) { // when the set is not full
        printf("Set is not full\n");
        fflush(stdout);
        int freeIndex;
        for(int i=0;i<8;i++) { // finding the free index 
            if(!hardware->l2->sets[setIndex].valid[i]){
                printf("Matched\n");
                fflush(stdout);
                freeIndex=i;
                break;
            }
        }
        printf("Free index: %d", freeIndex);
        fflush(stdout);
        hardware->l2->sets[setIndex].valid[freeIndex] = 1;
        hardware->l2->sets[setIndex].tags[freeIndex] = tag;
        
        if(hardware->l2->sets[setIndex].listCount == 0){
            printf("List Count 0\n");
            fflush(stdout);
            hardware->l2->sets[setIndex].head->index = freeIndex; // freeIndex will be 0 only
        }
        else {
            printf("List count is non zero\n");
            fflush(stdout);
            struct Node *node = (struct Node*) malloc(sizeof(struct Node));
            node->index = freeIndex;
            node->next = NULL;
            hardware->l2->sets[setIndex].tail->next = node;
            hardware->l2->sets[setIndex].tail = node;
        }

        hardware->l2->sets[setIndex].listCount++;
    }
    else { // when the set is full we use FIFO replacement policy
        printf("Set is full\n");
        fflush(stdout);
        struct Node *node = hardware->l2->sets[setIndex].head;
        dirty = hardware->l2->sets[setIndex].dirty[node->index];
        hardware->l2->sets[setIndex].dirty[node->index] = 0;
        int64_t oldpa = hardware->l2->sets[setIndex].tags[node->index];
        oldpa = (oldpa << 12) | (setIndex << 5);
        invalidateL1Line(oldpa, hardware);
        hardware->l2->sets[setIndex].tags[node->index] = tag;
        hardware->l2->sets[setIndex].head = hardware->l2->sets[setIndex].head->next;
        hardware->l2->sets[setIndex].tail->next = node;
        hardware->l2->sets[setIndex].tail = node;

    }
    printf("UpdateL2 ended\n");
    fflush(stdout);
    return dirty;
}

void writeBackL2(int64_t pa, struct Hardware *hardware) {
    int64_t setIndex = (pa >> 5) & (0x7f);
    int64_t tag = (pa >> 12) & (0x1fff);

    for(int i=0;i<8;i++) {
        if(hardware->l2->sets[setIndex].valid[i] && hardware->l2->sets[setIndex].tags[i] == tag) {
            printf("Setting the dirty bit\n");
            hardware->l2->sets[setIndex].dirty[i] = 1;
            return;
        }
    }

    return;
}

void invalidateL2Line(int64_t pa, struct Hardware *hardware) {
    int64_t setIndex = (pa >> 5) & (0x7f);
    int64_t tag = (pa >> 12) & (0x1fff);

    for(int i=0;i<8;i++) {
        if(hardware->l2->sets[setIndex].valid[i] && hardware->l2->sets[setIndex].tags[i] == tag) {
            printf("Invalidating line in L2\n");
            hardware->l2->sets[setIndex].valid[i] = 0;
            invalidateL1Line(pa, hardware);
            return;
        }
    }

    
    return;
}