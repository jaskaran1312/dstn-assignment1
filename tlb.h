long fetchTLB(struct Hardware *hardware, long pageNumber, int pid);
void invalidateTLB(struct Hardware *hardware, int pid);
void updateTLB(struct Hardware *hardware, long pageNumber, long frameNumber, int pid);