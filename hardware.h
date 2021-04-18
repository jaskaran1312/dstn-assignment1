#ifndef HARDWARE
#define HARDWARE

#include <inttypes.h>

struct Node {
	short index;
	struct Node *next;
};

struct L2CacheSet
{
	unsigned int tags[8];
	unsigned int dirty[8];
	unsigned int valid[8];
	struct Node *head;
	struct Node *tail;
	short listCount;
};

struct L1Cache
{
	unsigned int tags[128];
};

struct L2Cache
{
	struct L2CacheSet sets[128];
};

struct VictimCache
{
	unsigned int tags[8];
	unsigned short lruCounter[8];
	unsigned short valid[8];
};

struct FrameTable
{
	int pid[65536];
	int initialFrameAlloc;
};

struct PageTable
{
	unsigned short entries[256];
};

struct MainMemory
{
	unsigned int freeFrames;
	unsigned short nextFreeFrame;
	struct PageTable* frames[65536];

	uint16_t lru[65536];

};

struct TLB
{
	long pageNumber[32];
	long frameNumber[32];
	int pid[32];
	unsigned int valid[32];
	short lru[32][32];
};

struct Hardware
{
	struct L1Cache *l1;
	struct L2Cache *l2;
	struct VictimCache *victim;
	struct MainMemory *mainMemory;
	struct TLB *tlb;
	struct FrameTable *frametable;
};

struct SegmentTable
{
	unsigned short csBase;
	unsigned short dsBase;
	unsigned int csLength;
	unsigned int dsLength;
};

struct Process
{

	int pid;
	struct SegmentTable *ldt;
	short state; // Thrashing: 0 -> suspended, 1 -> active
};

#endif