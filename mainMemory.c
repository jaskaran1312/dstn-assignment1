#include "hardware.h"
#include "mainMemory.h"
#include "l1cache.h"
#include "l2cache.h"
#include "frametable.h"
#include <stdio.h>
#include <stdlib.h>

int64_t fetchMainMemory(int64_t la, int64_t pdpa, struct Hardware *hardware, struct Process *process) { //pdpa is the physical address of the page directory.
    printf("Fetching %lld from mm\n", la);
    fflush(stdout);
    //Returns -1 for Page Fault and datapa for Data Hit
    int64_t pdOffset = (la >> 17);
    int64_t ptOffset = (la >> 9) & (0xff);
    int64_t dataOffset = la & (0x1ff);

    int64_t ptpa = hardware->mainMemory->frames[pdpa]->entries[pdOffset]; //physical address of page table;
	printf("PTPA is %lld\n", ptpa);
    fflush(stdout);

    //Does the Page TABLE Frame belong to the process?
    if (checkFrameTable(ptpa, process, hardware)) {
        //Yes
        int64_t pagepa = hardware->mainMemory->frames[ptpa]->entries[ptOffset]; //physical address of data page;
        // Does the Page belong to the process?
        if (checkFrameTable(pagepa, process, hardware)) {

            int64_t datapa = (pagepa << 9) | dataOffset; //Physical address of data
            // Data Found
            return datapa;
        } else {
            int64_t pa = allocateNewFrame(process, hardware);
            hardware->mainMemory->frames[ptpa]->entries[ptOffset] = pa;
            return -1;
        }
    } else { // Page Fault for Page Table
        int64_t pa = allocateNewFrame(process, hardware);
        hardware->mainMemory->frames[pdpa]->entries[pdOffset] = pa;
        return -1;
    }
}
