#include <dirent.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "hardware.h"
#include "init.h"
#include "l1cache.h"
#include "l2cache.h"
#include "lru.h"
#include "mainMemory.h"
#include "segmentTable.h"
#include "thrashing.h"
#include "tlb.h"
#include "victimcache.h"

void fetchData(long pa, struct Hardware *hardware) {
    //TODO statistic variables

    //First try L1 cache
    if (!fetchL1Cache(pa, hardware))
        return; //Hit in L1

    //Miss in L1; try Victim cache
    if (!fetchVictimCache(pa, hardware)) {
        //Hit in Victim
        updateL1Cache(pa, hardware, 1);
        return;
    }

    //Miss in Victim; try L2
    if (!fetchL2Cache(pa, hardware)) {
        //Hit in L2
        updateL1Cache(pa, hardware, 0);
        return;
    }

    //Miss in L2
    updateL2Cache(pa, hardware);
    updateL1Cache(pa, hardware, 0);
}

// Process numbering starts from 0

void simulate(struct Hardware *hardware, char *fileList[], int numFiles) {
    int filePos[numFiles];

    for (int i = 0; i < numFiles; i++)
        filePos[i] = 0;

    FILE *fp[numFiles];

    for (int i = 0; i < numFiles; i++)
        fp[i] = fopen(fileList[i], "r");

    int processCompleted = 0;
    int currProcess = 0;
    uint32_t instructionCount = 0;

    struct Process *process[numFiles];
    for (int i = 0; i < numFiles; i++) {
        process[i] = (struct Process *)malloc(sizeof(struct Process));
        process[i]->pid = i;
        process[i]->state = 1;
        process[i]->ldt = (struct SegmentTable *)malloc(sizeof(struct SegmentTable));
        process[i]->ldt->csBase = -1;
        process[i]->ldt->csLength = -1;
        process[i]->ldt->dsBase = -1;
        process[i]->ldt->dsLength = -1;
    }

    while (1) {

        for (int i = 0; i < 2000; i++) {
            if (filePos[currProcess] == -1 || process[currProcess]->state == 0) {
                break;
            }

            instructionCount++;

            if (instructionCount % 5000 == 0)
                shiftMMLRU(hardware);
            if (instructionCount % 50000 == 0) {
                if (isThrashing(hardware)) {
                    short processToSuspend = rand() % numFiles;
                    process[processToSuspend]->state = 0;
                } else {
                    for (int i = 0; i < numFiles; i++) {
                        if (process[i]->state == 0)
                            process[i]->state = 1;
                    }
                }
            }

            char va[9];
            fseek(fp[currProcess], filePos[currProcess], SEEK_SET);
            if (fgets(va, 9, fp[currProcess]) == NULL) {
                printf("Process %d Completed\n", currProcess);
                invalidateTLB(hardware, currProcess); //invalidate tlb
                processCompleted++;
                filePos[currProcess] = -1;
                break;
            }
            filePos[currProcess] += 10;

            printf("Process: %d, Logical Address: %s\n", currProcess, va);

            // Fetch base from the segment table
            long pdpa = fetchBase(va, process[currProcess], hardware);
            // Fetch linear address
            long la = fetchLinearAddress(va);
            // Fetch physical address
            long pa = fetchMainMemory(la, pdpa, hardware, process[currProcess]);
            // Update TLB
            updateTLB(hardware, va, pa, process[currProcess]->pid);
            // Simulate in Memory management
            fetchData(pa, hardware);
        }

        currProcess = (currProcess + 1) % numFiles;

        if (processCompleted == numFiles) {
            printf("Simulation Completed\n");
            for (int i = 0; i < numFiles; i++)
                fclose(fp[i]);
            return;
        }
    }
}

int main() {

    struct Hardware *hardware = (struct Hardware *)malloc(sizeof(struct Hardware));

    tlbInit(hardware);         // tlb initialization
    l1CacheInit(hardware);     // l1cache initialization
    victimCacheInit(hardware); // victimcache intialization
    mainMemoryInit(hardware);  // mainmemory initialization
    l2CacheInit(hardware);     // l2cache initialization
    frameTableInit(hardware);  //frameTable initialization

    FILE *out = freopen("logs.txt", "w", stdout);
    DIR *dir;
    struct dirent *dirEntry;
    int numFiles = 0;

    dir = opendir("./InputFiles/");

    while ((dirEntry = readdir(dir)) != NULL) {
        if (dirEntry->d_type != DT_REG)
            continue;
        else
            numFiles++;
    }
    printf("Number of Input Files = %d\n", numFiles);

    rewinddir(dir);

    char *fileList[numFiles];
    int i = 0;

    while ((dirEntry = readdir(dir)) != NULL) {
        if (dirEntry->d_type != DT_REG)
            continue;
        else {
            char path[120] = "InputFiles/";
            fileList[i] = (char *)malloc(strlen(dirEntry->d_name) + strlen(path) + 1);
            strcat(path, dirEntry->d_name);
            strncpy(fileList[i], path, strlen(path));
            fileList[i][strlen(path)] = '\0';
            i++;
        }
    }
    if (closedir(dir) == -1) {
        perror("closedir");
        return 1;
    }

    for (int i = 0; i < numFiles; i++)
        printf("%s\n", fileList[i]);

    simulate(hardware, fileList, numFiles);

    fclose(out);
    return 0;
}
