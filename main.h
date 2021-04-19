#ifndef MAIN
#define MAIN
#include <inttypes.h>

// Statistics

typedef struct statsStruct {
    uint32_t tlbHit;
    uint32_t tlbReferences;
    uint32_t l1iHit;
    uint32_t l1dHit;
    uint32_t l1iReferences;
    uint32_t l1dReferences;
    uint32_t writeL1ToL2;
    uint32_t victimCacheHit;
    uint32_t victimCacheReferences;
    uint32_t l2Hit;
    uint32_t l2References;
    uint32_t writeL2toMM;
    uint32_t contextSwitch;
    uint32_t pageFault;
    uint8_t thrashing;
} stats;

#endif