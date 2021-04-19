#ifndef L2CACHE
#define L2CACHE

int fetchL2Cache (int64_t pa, struct Hardware *hardware);
int updateL2Cache(int64_t pa, struct Hardware *hardware);
void writeBackL2(int64_t pa, struct Hardware *hardware);
void invalidateL2Line(int64_t pa, struct Hardware *hardware);

#endif