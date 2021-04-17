#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <stdint.h>

#include "hardware.h"
#include "init.h"
#include "tlb.h"
#include "l1cache.h"
#include "victimcache.h"
#include "l2cache.h"
#include "lru.h"
#include "thrashing.h"

// Process numbering starts from 0

void simulate(struct Hardware *hardware, char *fileList[], int numFiles) {
	int filePos[numFiles];

	for(int i=0;i<numFiles;i++)
		filePos[i] = 0;

	FILE *fp[numFiles];

	for(int i=0;i<numFiles;i++) 
		fp[i] = fopen(fileList[i], "r");

	int processCompleted = 0;
	int currProcess = 0;
	uint32_t instructionCount = 0;
	
	while(1) {
		
		for(int i=0;i<2000;i++) {
			if(filePos[currProcess] == -1) {
				break;
			}

			instructionCount++;
			if(instructionCount%5000 == 0)
				shiftMMLRU(hardware);
			if(instructionCount%50000 == 0) {
				if(isThrashing(hardware)) {
					// TODO
				}
			}
				
			char va[9];
			fseek(fp[currProcess], filePos[currProcess], SEEK_SET);
			if(fgets(va, 9, fp[currProcess]) == NULL) {
				printf("Process %d Completed\n",currProcess);
				invalidateTLB(hardware, currProcess);//invalidate tlb
				processCompleted++;
				filePos[currProcess] = -1;
				break;
			}
			filePos[currProcess] += 10;

			printf("Process: %d, Logical Address: %s\n",currProcess, va);
			
			// TODO : 
			// get linear address
			// get physical address
			// simulate in memory management system

		}

		currProcess = (currProcess+1)%numFiles;
		
		if(processCompleted == numFiles) {
			printf("Simulation Completed\n");
			for(int i=0;i<numFiles;i++)
				fclose(fp[i]);
			return;
		}
	}

}

int main(){
	
	struct Hardware *hardware = (struct Hardware *) malloc(sizeof(struct Hardware));
	
	tlbInit(hardware); // tlb initialization
	l1CacheInit(hardware); // l1cache initialization
	victimCacheInit(hardware); // victimcache intialization
	l2CacheInit(hardware); // l2cache initialization
	
	FILE* out = freopen("logs.txt","w",stdout);
	
	DIR *dir;
	struct dirent *dirEntry;
	int numFiles = 0;

	dir = opendir("./InputFiles/");

	while ((dirEntry = readdir(dir)) != NULL)
	{
		if(dirEntry->d_type != DT_REG)
			continue;
		else
			numFiles++;
	}
	printf("Number of Input Files = %d\n", numFiles);

	rewinddir(dir);

	char *fileList[numFiles];
	int i=0;

	while ((dirEntry = readdir(dir)) != NULL)
	{
		if(dirEntry->d_type != DT_REG)
			continue;
		else {
			char path[120] = "InputFiles/";
			fileList[i] = (char *) malloc (strlen(dirEntry->d_name)+strlen(path)+1);
			strcat(path, dirEntry->d_name);
			strncpy (fileList[i], path, strlen(path));
			fileList[i][strlen(path)] = '\0';
			i++;
		}
	}
	if(closedir(dir) == -1) {
		perror("closedir");
		return 1;
	}
	
	for (int i = 0; i < numFiles; i++)
		printf("%s\n", fileList[i]);

	simulate(hardware, fileList, numFiles);
	
	fclose(out);
	return 0;

}
