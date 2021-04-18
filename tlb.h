int64_t fetchTLB(struct Hardware *hardware, int64_t pageNumber, int64_t pid);
void invalidateTLB(struct Hardware *hardware, int64_t pid);
void updateTLB(struct Hardware *hardware, int64_t pageNumber, int64_t frameNumber, int64_t pid);
void updateMatrix(struct Hardware *hardware, int64_t line);
void invalidateLine(struct Hardware *hardware, int64_t pageNumber, int64_t pid);