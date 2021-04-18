#include <stdio.h>
#include <stdint.h>
#include "hardware.h"

#define COUNT 15000
/*
To detect thrashing, we are using the same LRU bits of Main Memory.
The limitation here is the number of counters and so the approximation is as follows:
1) Right shift the LRU counters after every 5000 instrctions.
2) Check for thrashing after every 50k instructions.
3) Include in Working Set if the 10 most significant bit are non zero
4) If the size of Working Set is greater than 32k, thrashing detected.
*/

short isThrashing(struct Hardware *hardware) {
    int workingSetCount=0;
    for(int i=0;i<65536;i++) {
        if(((hardware->mainMemory->lru[i] >> 6) & 0x3FF) > 0)
            workingSetCount++; 
    }

    if(workingSetCount > COUNT) {
        printf("Thrashing Detected\n");
        return 1;
    }
    else {
        printf("No Thrashing Detected\n");
        return 0;
    }
        
}