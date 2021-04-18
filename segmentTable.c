#include "frametable.h"
#include "hardware.h"
#include <stdio.h>
#include <stdlib.h>

long fetchBase(long va, struct Process *process, struct Hardware *hardware) {

    uint16_t selector = va >> 24;
    int segment;
    if (selector == 0x7f) {
        if (!checkFrameTable(process->ldt->csBase, process, hardware)) {
            process->ldt->csBase = allocateNewFrame(process, hardware);
        }
        return process->ldt->csBase;
    } else if (selector == 0x10) {
        if (!checkFrameTable(process->ldt->dsBase, process, hardware)) {
            process->ldt->dsBase = allocateNewFrame(process, hardware);
        }
        return process->ldt->dsBase;
    } else {
        printf("Bad Address!\n");
        return -1;
    }
}

long fetchLinearAddress(long va) {
    return va & 0x00ffffff;
}