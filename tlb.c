#include<stdio.h>
#include<stdlib.h>
#include "hardware.h"
#include "tlb.h"

long fetchTLB(struct Hardware *hardware, long pageNumber, int pid) {
    printf("Checking in TLB...\n");
    for (int i=0;i<32;i++) {
        if(hardware->tlb->valid[i] && hardware->tlb->pid[i] == pid && hardware->tlb->pageNumber[i] == pageNumber) {
            printf("TLB hit for Process: %d & Page Number: %ld\n", pid,pageNumber);
            
            // updating LRU square matrix
            for(int j=0;j<32;j++)
                hardware->tlb->lru[i][j] = 1;
            for(int j=0;j<32;j++)
                hardware->tlb->lru[j][i] = 0;

            return hardware->tlb->frameNumber[i];
        }
    }
    printf("TLB miss for Process: %d & Page Number: %ld\n", pid,pageNumber);
    return -1; // tlb miss
}

void invalidateTLB(struct Hardware *hardware, int pid) {
    for(int i=0;i<32;i++) {
        if(hardware->tlb->pid[i] == pid)
            hardware->tlb->valid[i] = 0;
    }
}

void updateTLB(struct Hardware *hardware, long pageNumber, long frameNumber, int pid) {
    for(int i=0;i<32;i++) {
        if(!hardware->tlb->valid[i]) {
            printf("Found an invalid line!\nUpdating TLB...\n");
            hardware->tlb->pid[i] = pid;
            hardware->tlb->pageNumber[i] = pageNumber;
            hardware->tlb->frameNumber[i] = frameNumber;
            hardware->tlb->valid[i] = 1;
            printf("TLB updated\n");
            return;
        }
    }

    printf("All the lines are valid in TLB!\nFinding a line to evict...\n");
    
    for(int i=0;i<32;i++) {
        int zeroCount=0;
        for(int j=0;j<32;j++) {
            if(hardware->tlb->lru[i][j] == 0)
                zeroCount++;
        }
        if(zeroCount == 32) {
            printf("Found a line to evict, Process: %d, Page Number: %ld & Frame Number: %ld\n",hardware->tlb->pid[i], hardware->tlb->pageNumber[i], hardware->tlb->frameNumber[i]);
            hardware->tlb->pid[i] = pid;
            hardware->tlb->pageNumber[i] = pageNumber;
            hardware->tlb->frameNumber[i] = frameNumber;
            printf("TLB updated\n");
            return;
        }
    }

    return;
}