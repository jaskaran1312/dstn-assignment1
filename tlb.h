unsigned int fetchTLB(struct Hardware *hardware, unsigned int pageNumber, unsigned int pid);
void invalidateTLB(struct Hardware *hardware, unsigned int pid);