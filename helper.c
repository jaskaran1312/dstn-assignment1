#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include "main.h"
#include "hardware.h"

void saveResult(stats *statistics) {
    FILE *fptr = fopen("result.txt", "w");
    if (fptr == NULL) {
        printf("Could not open result.txt\n");
        return;
    }
    fprintf(fptr, "-----Statistics-----\n\n");
    fprintf(fptr, "Number of Hits in TLB: %" PRIu32 "\n", statistics->tlbHit);
    fprintf(fptr, "Number of References in TLB: %" PRIu32 "\n", statistics->tlbReferences);
    fprintf(fptr, "Hit Ratio in TLB: %f\n", (double)statistics->tlbHit / (double)statistics->tlbReferences);
    fprintf(fptr, "Number of Hits in L1 Data Cache: %" PRIu32 "\n", statistics->l1dHit);
    fprintf(fptr, "Number of References in L1 Data Cache: %" PRIu32 "\n", statistics->l1dReferences);
    fprintf(fptr, "Hit Ratio in L1 Data Cache: %f\n", (double)statistics->l1dHit / (double)statistics->l1dReferences);
    fprintf(fptr, "Number of Hits in L1 Instruction Cache: %" PRIu32 "\n", statistics->l1iHit);
    fprintf(fptr, "Number of References in L1 Instruction Cache: %" PRIu32 "\n", statistics->l1iReferences);
    fprintf(fptr, "Hit Ratio in L1 Instruction Cache: %f\n", (double)statistics->l1iHit / (double)statistics->l1iReferences);
    fprintf(fptr, "Number of Write-Through ops from L1 to L2: %" PRIu32 "\n", statistics->writeL1ToL2);
    fprintf(fptr, "Number of Hits in Victim Cache: %" PRIu32 "\n", statistics->victimCacheHit);
    fprintf(fptr, "Number of References in Victim Cache: %" PRIu32 "\n", statistics->victimCacheReferences);
    fprintf(fptr, "Hit Ratio in Victim Cache: %f\n", (double)statistics->victimCacheHit / (double)statistics->victimCacheReferences);
    fprintf(fptr, "Number of Hits in L2 Cache: %" PRIu32 "\n", statistics->l2Hit);
    fprintf(fptr, "Number of References in L2 Cache: %" PRIu32 "\n", statistics->l2References);
    fprintf(fptr, "Hit Ratio in L2 Cache: %f\n", (double)statistics->l2Hit / (double)statistics->l2References);
    fprintf(fptr, "Number of Write-Back ops from L2 to Main Memory: %" PRIu32 "\n", statistics->writeL2toMM);
    fprintf(fptr, "Number of Context Switches: %" PRIu32 "\n", statistics->contextSwitch);
    fprintf(fptr, "Number of Page Faults: %" PRIu32 "\n", statistics->pageFault);
    fprintf(fptr, "Number of times Thrashing occured: %" PRIu8 "\n", statistics->thrashing);

    fclose(fptr);
    return;
}

int fetchData(int64_t pa, struct Hardware *hardware, int selector, int makeRead, stats *statistics) {
    // If selector is zero we work with l1i else l1d

    //First try L1 cache
    if(!selector) statistics->l1iReferences++;
    else statistics->l1dReferences ++;
    if (!fetchL1Cache(pa, hardware, selector)){
			if(!selector) statistics->l1iHit++;
            else statistics->l1dHit ++;
            printf("L1 Hit\n");
            if(!makeRead){
                //Write through to L2
                printf("Write through to L2\n");
                writeBackL2(pa, hardware);
                statistics->writeL1ToL2++;
            }
	      	return 1; //Hit in L1
	}
  

    //Miss in L1; try Victim cache
    statistics->victimCacheReferences++;
    if (!fetchVictimCache(pa, hardware)) {
        //Hit in Victim
        statistics->victimCacheHit++;
        printf("Victim Hit\n");
        updateL1Cache(pa, hardware, 1, selector);
        return 0;
    }

    //Miss in Victim; try L2
    statistics->l2References++;
    if (!fetchL2Cache(pa, hardware)) {
        //Hit in L2
        statistics->l2Hit++;
        printf("L2 Hit\n");
        updateL1Cache(pa, hardware, 0, selector);
        return 0;
    }
    fflush(stdout);
    //Miss in L2
    statistics->writeL2toMM += updateL2Cache(pa, hardware);
    updateL1Cache(pa, hardware, 0, selector);
    return 0;
}

int readWriteSelector(int selector) {
    if (selector == 1) { // Choosing write instructions from Data Segment
        int makeRead = rand()%2;
        printf("makeRead is %d\n", makeRead);
        return makeRead;
    }
    return 1;
}