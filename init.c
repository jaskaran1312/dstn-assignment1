#include<stdio.h>
#include<stdlib.h>
#include"hardware.h"
#include"init.h"

void tlbInit(struct Hardware *hardware) {
	hardware->tlb = (struct TLB *) malloc(sizeof(struct TLB));
	for(int i=0;i<32;i++) {
		hardware->tlb->frameNumber[i] = -1;
		hardware->tlb->pageNumber[i] = -1;
		hardware->tlb->pid[i] = -1;
		hardware->tlb->valid[i] = 0;
	}
}

void l1CacheInit(struct Hardware *hardware) {
    hardware->l1 = (struct L1Cache *) malloc(sizeof(struct L1Cache));
	for(int i=0;i<128;i++) {
		hardware->l1->tags[i] = 0;
		hardware->l1->valid[i] = 0;
	}
}

void victimCacheInit(struct Hardware *hardware) {
    hardware->victim = (struct VictimCache *) malloc(sizeof(struct VictimCache));
	for(int i=0;i<8;i++)
	{
		hardware->victim->tags[i]=0;
		hardware->victim->lruCounter[i]=0;
		hardware->victim->valid[i]=0;
	}
}

void l2CacheInit(struct Hardware *hardware) {
    hardware->l2 = (struct L2Cache *) malloc(sizeof(struct L2Cache));
	for(int i=0;i<128;i++) {
		hardware->l2->sets[i].listCount = 0;
		struct Node *node = (struct Node *) malloc(sizeof(struct Node));
		node->next = NULL;
		hardware->l2->sets[i].head = node;
		hardware->l2->sets[i].tail = node;
		for(int j=0;j<8;j++) {
			hardware->l2->sets[i].valid[j] = 0;
			hardware->l2->sets[i].tags[j] = 0;
			hardware->l2->sets[i].dirty[j] = 0;
		}
	}
}