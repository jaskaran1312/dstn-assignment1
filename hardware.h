#ifndef HARDWARE
#define HARDWARE

#include <inttypes.h>

struct Node {
	int64_t index;
	struct Node *next;
};

struct L2CacheSet
{
	int64_t tags[8];
	int64_t dirty[8];
	int64_t valid[8];
	struct Node *head;
	struct Node *tail;
	int64_t listCount;
};

struct L1Cache
{
	int64_t tags[128];
};

struct L2Cache
{
	struct L2CacheSet sets[128];
};

struct VictimCache
{
	int64_t tags[8];
	int64_t lruCounter[8];
	int64_t valid[8];
};

struct FrameTable
{
	int64_t pid[65536];
	int64_t initialFrameAlloc;
};

struct PageTable
{
	int64_t entries[256];
};

struct MainMemory
{
	int64_t freeFrames;
	int64_t nextFreeFrame;
	struct PageTable* frames[65536];

	int64_t lru[65536];

};

struct TLB
{
	int64_t pageNumber[32];
	int64_t frameNumber[32];
	int64_t pid[32];
	int64_t valid[32];
	int64_t lru[32][32];
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
	int64_t csBase;
	int64_t dsBase;
	int64_t csLength;
	int64_t dsLength;
};

struct Process
{

	int64_t pid;
	struct SegmentTable *ldt;
	int64_t state; // Thrashing: 0 -> suspended, 1 -> active
};

#endif