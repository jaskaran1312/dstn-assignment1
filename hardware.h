struct Node {
	short index;
	Node* next;
}

struct L2CacheSet {
	unsigned int tags[8];
	unsigned int dirty[8];
	unsigned int valid[8];
	struct Node *head;
	struct Node *tail;
	short listCount;

}

struct L1Cache {
	unsigned int tags[128];
	unsigned short valid[128];
}

struct L2Cache {
	struct L2CacheSet sets[128];
}

struct VictimCache {
	unsigned int tags[8];
	unsigned short lruCounter[8];
	unsigned short valid[8];
}

struct MainMemory {
	//TODO
}

struct Hardware {
	
	struct L1Cache *l1;
	struct L2Cache *l2;
	struct VictimCache *victim;
	struct MainMemory *mainMemory;

}
