#include<stdio.h>
#include<stdlib.h>
#include "hardware.h"

//assumption: both lru and frames are initialized to 0


//returns 1 if frame belongs to process, 0 otherwise
int checkFrameTable (long pa, struct Process *process, struct FrameTable *frametable)
{
    int index= (pa>>9) & (0b1111111111111111);

    if(frametable->pid[index]==process->pid)
    {
        return 1;
    }
    return 0;
}

int allocateNewFrame(struct Process *process, struct Hardware *hardware, struct FrameTable *frametable)
{
    
    int temp;
    //freeframes stores no. of free frames
    if(hardware->mainMemory->freeFrames > 0)
    {
        temp=hardware->mainMemory->nextFreeFrame;
        hardware->mainMemory->freeFrames --;

        //todo: update frametable with pid
        frametable->pid[temp]=process->pid;

        findNextFreeFrame(temp, hardware);
    }
    else
    {
        temp=findLRU();
    }
    updateLRU(temp);
    return temp;

}

void findNextFreeFrame(int temp, struct Hardware *hardware)
{
    int i=temp++;
    
    while(i!=temp)
    {
        if(hardware->mainMemory->frames[i]==0)
        {
            hardware->mainMemory->frames[i];
            return;
        }

        i++;
        if(i==65536) //double check this value please
        {
            i=0;
        }

    }
}