#include <stdio.h>
#include <stdlib.h>
#include "hardware.h"
#include "l1cache.h"
#include "victimcache.h"

int64_t fetchL1Cache(int64_t pa, struct Hardware *hardware)
{
    int64_t index = (pa >> 5) & (0x7f);
    int64_t tag = (pa >> 12) & (0x1fff);

    if (hardware->l1->tags[index] == tag)
        return 0; // hit

    return 1; //miss
}

void updateL1Cache(int64_t pa, struct Hardware *hardware, int fromVictim)
{
    //fromVictim is 1 if hit in victim (==MRU replacement), else 0(==LRU replacement)
    int64_t index = (pa >> 5) & (0b1111111);
    int64_t tag = (pa >> 12) & (0b1111111111111);

    int64_t oldpa = 0;
    //reconstructing oldpa but without offset bits
    if (hardware->l1->tags[index] != -1) //-1 => empty => placement => no need to update victim cache
    {
        oldpa = oldpa | (hardware->l1->tags[index] << 12) | (index << 5);
        updateVictimCache(oldpa, hardware, !fromVictim);
    }

    //set tag bits
    hardware->l1->tags[index] = tag;
}
