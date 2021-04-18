#include <stdio.h>
#include <stdlib.h>
#include "hardware.h"
#include "victimcache.h"

int fetchVictimCache(long pa, struct Hardware *hardware)
{
    int tag = (pa >> 5) & (0b11111111111111111111);

    for (int i = 0; i < 8; i++)
    {
        if (hardware->victim->tags[i] == tag && hardware->victim->valid[i] == 1)
        {
            updateLRU(hardware, i);
            return 0;
        }
    }
    return 1;
}

void updateVictimCache(long pa, struct Hardware *hardware, int method)
{
    //method=0 implies MRU replacement, method=1 implies LRU replacement
    if (method == 0)
    {
        for (int i = 0; i < 8; i++)
        {
            if (hardware->victim->lruCounter[i] == 7)
            {
                //replace MRU element which has been moved to L1 (because hit)
                hardware->victim->tags[i] = (pa >> 5) & (0b11111111111111111111);
                hardware->victim->valid[i] = 1;
                break;
            }
        }
    }
    else
    {
        for (int i = 0; i < 8; i++)
        {
            if (hardware->victim->lruCounter[i] == 0)
            {
                hardware->victim->tags[i] = (pa >> 5) & (0b11111111111111111111);
                hardware->victim->valid[i] = 1;
                updateLRU(hardware, i);
                //make this new element MRU
                break;
            }
        }
    }
}

void updateLRU(struct Hardware *hardware, short index)
{
    short temp = hardware->victim->lruCounter[index];
    for (int i = 0; i < 8; i++)
    {
        if (hardware->victim->lruCounter[i] > temp)
        {
            hardware->victim->lruCounter[i]--;
        }
    }
    hardware->victim->lruCounter[index] = 7;
    //this value assignment needs to be done after,
    //otherwise its value will be decremented in for loop.
}