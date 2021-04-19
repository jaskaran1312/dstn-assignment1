int64_t fetchL1Cache(int64_t pa, struct Hardware *hardware, int selector);
void updateL1Cache(int64_t pa, struct Hardware *hardware, int fromVictim, int selector);
void invalidateL1Line(int64_t pa, struct Hardware *hardware, int selector);
