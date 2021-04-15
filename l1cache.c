#include<stdio.h>
#include<stdlib.h>
#include "hardware.h"
#include "l1cache.h"

long fetchL1Cache (long pa, struct Hardware *hardware) {
    int index = (pa >> 5) & (0b1111111);
    int tag = (pa >> 12) & (0b1111111111111);

    if(hardware->l1->tags[index] == tag)
	    return 0; // hit
    
    long oldpa = 0;
    //reconstructing oldpa but without offset bits
    oldpa = oldpa | (hardware->l1->tags[index] << 12) | (index << 5);
    return oldpa; //miss
}

void updateL1Cache(long pa, struct Hardware *hardware) {
    int index = (pa >> 5) & (0b1111111);
    int tag = (pa >> 12) & (0b1111111111111);
    
    //set tag bits
    hardware->l1->tags[index] = tag;

}
