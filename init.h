void tlbInit(struct Hardware *hardware);
void l1CacheInit(struct Hardware *hardware);
void victimCacheInit(struct Hardware *hardware);
void l2CacheInit(struct Hardware *hardware);
void frameTableInit(struct Hardware *hardware);
void mainMemoryInit(struct Hardware *hardware);
void processInit(struct Process *process, int pid);