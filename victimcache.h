#ifndef VC
#define VC
int fetchVictimCache(int64_t pa, struct Hardware *hardware);
void updateVictimCache(int64_t pa, struct Hardware *hardware, int method);
void updateLRU(struct Hardware *hardware, short index);
#endif