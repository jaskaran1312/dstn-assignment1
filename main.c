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
#include "helper.h"



void prePage(struct Hardware *hardware, char *fileList[], int numFiles) 
{
    int filePos[numFiles];

    //will be reinitialised in simulation, so don't need to reset later
    for (int i = 0; i < numFiles; i++)
        filePos[i] = 0;

    FILE *fp[numFiles];

    for (int i = 0; i < numFiles; i++)
        fp[i] = fopen(fileList[i], "r");

    // int processCompleted = 0;
    int currProcess = 0;
   

    struct Process *process[numFiles];
    for (int i = 0; i < numFiles; i++)
        process[i] = processInit(i);

    printf("PREPAGING BEGINS. . .\n");


    for(int jk=0;jk<numFiles;jk++) //iterate through files
    {
        printf("entering prepaging\n");
		fflush(stdout);
        for (int i = 0; i < 2; i++) //run for first 2 instructions of each process
        {
        
            printf("entering prepaging\n");
			fflush(stdout);

            char va[9];
            fseek(fp[currProcess], filePos[currProcess], SEEK_SET);

            if (fgets(va, 9, fp[currProcess]) == NULL) 
            {
                //WILL BE ENTERED IF BY SOME MIRACLE, A PROCESS IS 2 INSTRUCTIONS LONG
                printf("Process %d Completed\n", currProcess);
                invalidateTLB(hardware, currProcess); //invalidate tlb
                // process Completed
                filePos[currProcess] = -1;
                break;
            }

            filePos[currProcess] += 10;

            printf("Process: %d, Logical Address: %s\n", currProcess, va);
			fflush(stdout);
            int64_t virtualAddress;
            sscanf(va, "%lx", &virtualAddress);

            printf("Virtual address: %ld\n", virtualAddress);
			fflush(stdout);

            // Fetch base from the segment table
            int64_t pdpa = fetchBase(virtualAddress, process[currProcess], hardware);
			printf("Base address %ld\n", pdpa);
            fflush(stdout);
			
			// Fetch linear address
            int64_t la = fetchLinearAddress(virtualAddress);
			printf("Linear address %ld\n", la);
			fflush(stdout);

            // tlbReferences++;
            int64_t pa = fetchTLB(hardware, virtualAddress, process[currProcess]->pid);
            printf("pa from tlb %ld\n", pa);
			fflush(stdout);
            // tlbHit++;

            // Fetch physical address
            if (pa == -1){
                // tlbHit--;

                while(pa == -1){
                    
                    pa = fetchMainMemory(la, pdpa, hardware, process[currProcess]);
                    // pageFault++;
                    printf("PAGE FAULT\n");
			        fflush(stdout);
                }
                    
            }
            
            printf("Physical address %ld\n", pa);
			fflush(stdout);
			
			// Update TLB
            updateTLB(hardware, virtualAddress, pa, process[currProcess]->pid);

			fflush(stdout);
        }

        //context switch
        currProcess = (currProcess + 1) % numFiles;
        
    }

    for (int i = 0; i < numFiles; i++)
        fclose(fp[i]);

    printf("PREPAGING ENDS.\n");
    fflush(stdout);

    return;
        
}


// Process numbering starts from 0

void simulate(struct Hardware *hardware, char *fileList[], int numFiles, stats *statistics) {

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
                    statistics->thrashing++;
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
            int64_t virtualAddress;
            sscanf(va, "%lx", &virtualAddress);

            printf("Virtual address: %ld\n", virtualAddress);
			fflush(stdout);

            // selector = 0 for code segment, 1 for data segment 
            int selector = fetchSegment(virtualAddress);

            int makeRead = 1; // 1 -> read, 0 -> write
            makeRead = readWriteSelector(selector);

            // Fetch base from the segment table
            int64_t pdpa = fetchBase(virtualAddress, process[currProcess], hardware);
			printf("Base address %ld\n", pdpa);
            fflush(stdout);
			
			// Fetch linear address
            int64_t la = fetchLinearAddress(virtualAddress);
			printf("Linear address %ld\n", la);
			fflush(stdout);

            statistics->tlbReferences++;
            int64_t pa = fetchTLB(hardware, virtualAddress, process[currProcess]->pid);
            printf("pa from tlb %ld\n", pa);
			fflush(stdout);
            statistics->tlbHit++;
            // Fetch physical address
            if (pa == -1){
                statistics->tlbHit--;

                while(pa == -1){
                    
                    pa = fetchMainMemory(la, pdpa, hardware, process[currProcess]);
                    statistics->pageFault++;
                    printf("PAGE FAULT\n");
			        fflush(stdout);
                }
                    
            }
            
            printf("Physical address %ld\n", pa);
			fflush(stdout);
			
			// Update TLB
            updateTLB(hardware, virtualAddress, pa, process[currProcess]->pid);
            // Simulate in Memory management
			fflush(stdout);
            int dataFound = 0;
            while(!dataFound){
                dataFound = fetchData(pa, hardware, selector, makeRead, statistics);
            }
            
			printf("Got Data\n");
			fflush(stdout);
            
        }

        statistics->contextSwitch++;
        currProcess = (currProcess + 1) % numFiles;

        if (processCompleted == numFiles) {
            printf("Simulation Completed\n");
            for (int i = 0; i < numFiles; i++)
                fclose(fp[i]);
            saveResult(statistics);
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
    stats *statistics = (stats *) malloc(sizeof(stats));
    statsInit(statistics);                //statistics initialization

    printf("##############################################\n");
    printf("########## Memory Management System ##########\n");
    printf("##############################################\n\n");
    printf("For Debugging, check logs.txt\nFor Result, check result.txt\n\n");
    printf("Simulation Running...\n\n");

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
    
    prePage(hardware, fileList, numFiles); //prepaging
    fflush(stdout);

    simulate(hardware, fileList, numFiles, statistics);

    fclose(out);

    freopen("/dev/tty", "a", stdout);
    printf("Simulation Completed!\n");
    return 0;
}
