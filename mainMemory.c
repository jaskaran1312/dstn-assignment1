#include "hardware.h"
#include "mainMemory.h"
#include "l1cache.h"
#include "l2cache.h"
#include "frametable.h"
#include <stdio.h>
#include <stdlib.h>

long fetchMainMemory(long la, long pdpa, struct Hardware *hardware, struct Process *process) { //pdpa is the physical address of the page directory.
    //Returns -1 for Page Fault and datapa for Data Hit
    int pdOffset = (la >> 17);
    int ptOffset = (la >> 9) & (0xff);
    int dataOffset = la & (0x1ff);

    short ptpa = hardware->mainMemory->frames[pdpa]->entries[pdOffset]; //physical address of page table;
	fflush(stdout);

    //Does the Page TABLE Frame belong to the process?
    if (checkFrameTable(ptpa, process, hardware)) {
        //Yes
        short pagepa = hardware->mainMemory->frames[ptpa]->entries[ptOffset]; //physical address of data page;
        // Does the Page belong to the process?
        if (checkFrameTable(pagepa, process, hardware)) {

            long datapa = (pagepa << 9) | dataOffset; //Physical address of data
            // Data Found
            return datapa;
        } else {
            short pa = allocateNewFrame(process, hardware);
            hardware->mainMemory->frames[ptpa]->entries[ptOffset] = pa;
            return -1;
        }
    } else { // Page Fault for Page Table
        short pa = allocateNewFrame(process, hardware);
        hardware->mainMemory->frames[pdpa]->entries[pdOffset] = pa;
        return -1;
    }
}
