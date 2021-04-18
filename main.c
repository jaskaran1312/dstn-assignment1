#include <dirent.h>
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "hardware.h"
#include "frametable.h"
#include "init.h"
#include "l1cache.h"
#include "l2cache.h"
#include "lru.h"
#include "main.h"
#include "mainMemory.h"
#include "segmentTable.h"
#include "thrashing.h"
#include "tlb.h"
#include "victimcache.h"

void saveResult() {
    FILE *fptr = fopen("result.txt", "w");
    if (fptr == NULL) {
        printf("Could not open result.txt\n");
        return;
    }
    fprintf(fptr, "-----Statistics-----\n\n");
    fprintf(fptr, "Number of Hits in TLB: %" PRIu32 "\n", tlbHit);
    fprintf(fptr, "Number of References in TLB: %" PRIu32 "\n", tlbReferences);
    fprintf(fptr, "Hit Ratio in TLB: %f\n", (double)tlbHit / (double)tlbReferences);
    fprintf(fptr, "Number of Hits in L1 Cache: %" PRIu32 "\n", l1Hit);
    fprintf(fptr, "Number of References in L1 Cache: %" PRIu32 "\n", l1References);
    fprintf(fptr, "Hit Ratio in L1 Cache: %f\n", (double)l1Hit / (double)l1References);
    fprintf(fptr, "Number of Hits in Victim Cache: %" PRIu32 "\n", victimCacheHit);
    fprintf(fptr, "Number of References in Victim Cache: %" PRIu32 "\n", victimCacheReferences);
    fprintf(fptr, "Hit Ratio in Victim Cache: %f\n", (double)victimCacheHit / (double)victimCacheReferences);
    fprintf(fptr, "Number of Hits in L2 Cache: %" PRIu32 "\n", l2Hit);
    fprintf(fptr, "Number of References in L2 Cache: %" PRIu32 "\n", l2References);
    fprintf(fptr, "Hit Ratio in L2 Cache: %f\n", (double)l2Hit / (double)l2References);
    fprintf(fptr, "Number of Context Switches: %" PRIu32 "\n", contextSwitch);
    fprintf(fptr, "Number of Page Faults: %" PRIu32 "\n", pageFault);
    fprintf(fptr, "Number of times Thrashing occured: %" PRIu8 "\n", thrashing);

    fclose(fptr);

}

void fetchData(long pa, struct Hardware *hardware) {

    //First try L1 cache
    l1References++;
    if (!fetchL1Cache(pa, hardware)){
			l1Hit++;
            printf("L1 Hit\n");
	      	return; //Hit in L1
	}
  

    //Miss in L1; try Victim cache
    victimCacheReferences++;
    if (!fetchVictimCache(pa, hardware)) {
        //Hit in Victim
        victimCacheHit++;
        printf("Victim Hit\n");
        updateL1Cache(pa, hardware, 1);
        return;
    }

    //Miss in Victim; try L2
    l2References++;
    if (!fetchL2Cache(pa, hardware)) {
        //Hit in L2
        l2Hit++;
        printf("L2 Hit\n");
        updateL1Cache(pa, hardware, 0);
        return;
    }
    fflush(stdout);
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
    for (int i = 0; i < numFiles; i++)
        process[i] = processInit(i);


    while (1) {
			fflush(stdout);
        for (int i = 0; i < 2000; i++) {
            if (filePos[currProcess] == -1  || process[currProcess]->state == 0 ) {
                break;
            }

			fflush(stdout);

            instructionCount++;

            if (instructionCount % 5000 == 0)
                shiftMMLRU(hardware);
            if (instructionCount % 50000 == 0) {
                if (isThrashing(hardware)) {
                    thrashing++;
                    short processToSuspend = rand() % numFiles;
                    process[processToSuspend]->state = 0;
                } else {
                    for (int i = 0; i < numFiles; i++) {
                        if (process[i]->state == 0)
                            process[i]->state = 1;
                    }
                }
            }

			printf("Instruction count %" PRIu32 "\n", instructionCount);
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
			fflush(stdout);
            long virtualAddress;
            sscanf(va, "%lx", &virtualAddress);

            printf("Virtual address: %ld\n", virtualAddress);
			fflush(stdout);

            // Fetch base from the segment table
            long pdpa = fetchBase(virtualAddress, process[currProcess], hardware);
			fflush(stdout);
			
			// Fetch linear address
            long la = fetchLinearAddress(virtualAddress);
			printf("Linear address %ld\n", la);
			fflush(stdout);

            tlbReferences++;
            long pa = fetchTLB(hardware, virtualAddress, process[currProcess]->pid);
            
            tlbHit++;
            // Fetch physical address
            if (pa == -1){
                tlbHit--;

                while(pa==-1){
                    pa = fetchMainMemory(la, pdpa, hardware, process[currProcess]);
                    pageFault++;
                }
                    
            }
            
            
            printf("Physical address %ld\n", pa);
			fflush(stdout);
			
			// Update TLB
            updateTLB(hardware, virtualAddress, pa, process[currProcess]->pid);
            // Simulate in Memory management
			fflush(stdout);
            fetchData(pa, hardware);
			printf("Got Data\n");
			fflush(stdout);
            
        }

        contextSwitch++;
        currProcess = (currProcess + 1) % numFiles;

        if (processCompleted == numFiles) {
            printf("Simulation Completed\n");
            for (int i = 0; i < numFiles; i++)
                fclose(fp[i]);
            saveResult();
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

	fflush(stdout);
    simulate(hardware, fileList, numFiles);

    fclose(out);
    return 0;
}
