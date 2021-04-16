#include<stdio.h>
#include<stdlib.h>
#include "hardware.h"
#include "tlb.h"

long fetchTLB(struct Hardware *hardware, long pageNumber, int pid) {
    printf("Checking in TLB...\n");
    for (int i=0;i<32;i++) {
        if(hardware->tlb->valid[i] && hardware->tlb->pid[i] == pid && hardware->tlb->pageNumber[i] == pageNumber) {
            printf("TLB hit for Process: %d & Page Number: %ld\n", pid,pageNumber);            
            updateMatrix(hardware, i); // updating LRU square matrix
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
            updateMatrix(hardware, i);
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
            updateMatrix(hardware, i);
            printf("TLB updated\n");
            return;
        }
    }

    return;
}

void updateMatrix(struct Hardware *hardware, int line) { 
    for(int j=0;j<32;j++)
        hardware->tlb->lru[line][j] = 1;
    for(int j=0;j<32;j++)
        hardware->tlb->lru[j][line] = 0;
    return;
}

void invalidateLine(struct Hardware *hardware, long pageNumber, int pid) {
    for(int i=0;i<32;i++) {
        if(hardware->tlb->pid[i] == pid && hardware->tlb->pageNumber[i] == pageNumber) {
            printf("Found the line in TLB to invalidate!\n");
            hardware->tlb->valid[i] = 0;
            printf("Invalidated the line with Process: %d and Page Number: %ld\n", pid, pageNumber);
            return;
        }
    }
    printf("Line not found in TLB to invalidate\n");
    return;
}