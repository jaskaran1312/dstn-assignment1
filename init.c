#include <stdio.h>
#include <stdlib.h>
#include "hardware.h"
#include "init.h"

void tlbInit(struct Hardware *hardware)
{
	hardware->tlb = (struct TLB *)malloc(sizeof(struct TLB));
	for (int i = 0; i < 32; i++)
	{
		hardware->tlb->frameNumber[i] = -1;
		hardware->tlb->pageNumber[i] = -1;
		hardware->tlb->pid[i] = -1;
		hardware->tlb->valid[i] = 0;
		for (int j = 0; j < 32; j++)
			hardware->tlb->lru[i][j] = -1;
	}
}

void l1CacheInit(struct Hardware *hardware)
{
	hardware->l1i = (struct L1Cache *)malloc(sizeof(struct L1Cache));
	for (int i = 0; i < 128; i++)
	{
		hardware->l1i->tags[i] = -1;
		hardware->l1i->valid[i] = 0;
	}

	hardware->l1d = (struct L1Cache *)malloc(sizeof(struct L1Cache));
	for (int i = 0; i < 128; i++)
	{
		hardware->l1d->tags[i] = -1;
		hardware->l1d->valid[i] = 0;
	}
}

void victimCacheInit(struct Hardware *hardware)
{
	hardware->victim = (struct VictimCache *)malloc(sizeof(struct VictimCache));
	for (int i = 0; i < 8; i++)
	{
		hardware->victim->tags[i] = 0;
		hardware->victim->lruCounter[i] = 0;
		hardware->victim->valid[i] = 0;
	}
}

void l2CacheInit(struct Hardware *hardware)
{
	hardware->l2 = (struct L2Cache *)malloc(sizeof(struct L2Cache));
	for (int i = 0; i < 128; i++)
	{
		hardware->l2->sets[i].listCount = 0;
		struct Node *node = (struct Node *)malloc(sizeof(struct Node));
		node->next = NULL;
		hardware->l2->sets[i].head = node;
		hardware->l2->sets[i].tail = node;
		for (int j = 0; j < 8; j++)
		{
			hardware->l2->sets[i].valid[j] = 0;
			hardware->l2->sets[i].tags[j] = 0;
			hardware->l2->sets[i].dirty[j] = 0;
		}
	}
}


void frameTableInit(struct Hardware *hardware)
{
	hardware->frametable = (struct FrameTable *) malloc(sizeof(struct FrameTable));
	hardware->frametable->initialFrameAlloc =0;
	//have initialized all pids to -1, let me know if you want to change it.
	for(int i=0;i<65536;i++)
	{
		hardware->frametable->pid[i]=-1;
	}
}

void mainMemoryInit(struct Hardware *hardware)
{
	hardware->mainMemory = (struct MainMemory *)malloc(sizeof(struct MainMemory));
	for (int i = 0; i < 65536; i++)
	{
		hardware->mainMemory->frames[i] = NULL;
		hardware->mainMemory->lru[i] = 0;
	}
	hardware->mainMemory->freeFrames = 65536;
	hardware->mainMemory->nextFreeFrame = 0;

}

struct Process *processInit(int64_t pid) 
{
	struct Process *process = (struct Process *)malloc(sizeof(struct Process));
	process->pid = pid;
	process->state = 1;
	process->ldt = (struct SegmentTable *)malloc(sizeof(struct SegmentTable));
	process->ldt->csBase = -1;
	process->ldt->csLength = -1;
	process->ldt->dsBase = -1;
	process->ldt->dsLength = -1;
	return process;
}