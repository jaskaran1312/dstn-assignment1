int fetchVictimCache(long pa, struct Hardware *hardware);
void updateVictimCache(long pa, struct Hardware *hardware, int method);
void updateLRU(struct Hardware *hardware, short index);