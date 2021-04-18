#include "frametable.h"
#include "hardware.h"
#include <stdio.h>
#include <stdlib.h>

int64_t fetchBase(int64_t va, struct Process *process, struct Hardware *hardware) {
    
    int64_t selector = va >> 24;
    printf("selector %ld va %ld\n", selector, va);
    if (selector == (int64_t)0x7f) {
        printf("Code Segment\n");
        fflush(stdout);
        if (!checkFrameTable(process->ldt->csBase, process, hardware)) {
            process->ldt->csBase = allocateNewFrame(process, hardware);
        }
        return process->ldt->csBase;
    } else if (selector == (int64_t)0x10) {
        if (!checkFrameTable(process->ldt->dsBase, process, hardware)) {
            process->ldt->dsBase = allocateNewFrame(process, hardware);
        }
        return process->ldt->dsBase;
    } else {
        printf("Bad Address!\n");
        return -1;
    }
}

int64_t fetchLinearAddress(int64_t va) {
    return va & 0x00ffffff;
}