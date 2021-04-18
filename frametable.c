#include "frametable.h"
#include "hardware.h"
#include "lru.h"
#include <stdio.h>
#include <stdlib.h>

//returns 1 if frame belongs to process, 0 otherwise
int checkFrameTable(long pa, struct Process *process, struct Hardware *hardware) {

    if (hardware->frametable->pid[pa] == process->pid) {
        return 1;
    }
    return 0;
}

int allocateNewFrame(struct Process *process, struct Hardware *hardware) {
    /*
    basically, because there is no "freeing" of frames, each frame will only be empty once.
    This means I only have to allocate each new frame once, which I'm doing linearly.
    This also means we dont need free frameframecount, so I removed it.
    */
    int temp;

    if (hardware->frametable->initialFrameAlloc < 65536) //there are free frames
    {
        temp = hardware->frametable->initialFrameAlloc; // allocate next free frame
        hardware->frametable->initialFrameAlloc++;
    } else {
        temp = getMMLRU(hardware); //allocate LRU
    }

    hardware->frametable->pid[temp] = process->pid;
    //allocated/replaced frame, have not cleaned it though

    updateMMLRU(hardware, temp);
    return temp;
}

// fetchSegmentTableEntry()