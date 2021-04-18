#include <inttypes.h>

// Statistics
int64_t tlbHit=0;
int64_t tlbReferences=0;
int64_t l1Hit=0;
int64_t l1References=0;
int64_t victimCacheHit=0;
int64_t victimCacheReferences=0;
int64_t l2Hit=0;
int64_t l2References=0;
int64_t contextSwitch=0;
int64_t pageFault=0;
uint8_t thrashing=0;
