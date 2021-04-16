#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>

#include "hardware.h"
#include "init.h"
#include "tlb.h"
#include "l1cache.h"
#include "victimcache.h"
#include "l2cache.h"

void fetchData(long pa, struct Hardware *hardware)
{
	//TODO statistic variables
	long retVal; //Will hold the PA of line evicted from L1Cache in case of miss

	//First try L1 cache
	retVal = fetchL1Cache(pa, hardware);
	if (!retVal)
		return; //Hit in L1

	//Miss in L1; try Victim cache
	if (!fetchVictimCache(pa, hardware))
	{
		//Hit in Victim
		updateVictimCache(retVal, hardware, 0);
		updateL1Cache(pa, hardware);
		return;
	}

	//Miss in Victim; try L2
	if (!fetchL2Cache(pa, hardware))
	{
		//Hit in L2
		updateL1Cache(pa, hardware);
		updateVictimCache(retVal, hardware, 1);
		return;
	}

	//Miss in L2
}

int main()
{

	struct Hardware *hardware = (struct Hardware *)malloc(sizeof(struct Hardware));

	//l1cache initialization
	hardware->l1 = (struct L1Cache *)malloc(sizeof(struct L1Cache));
	for (int i = 0; i < 128; i++)
	{
		hardware->l1->tags[i] = 0;
	}

	//l2cache initialization
	hardware->l2 = (struct L2Cache *)malloc(sizeof(struct L2Cache));
	for (int i = 0; i < 128; i++)
	{
		hardware->l2->sets[i].listCount = 0;
		struct Node *node = (struct Node *)malloc(sizeof(struct Node));
		node->next = NULL;
		hardware->l2->sets[i].head = node;
		hardware->l2->sets[i].tail = node;
		for (int j = 0; j < 8; j++)
		{
			hardware->l2->sets[i].valid[j] = 0;
			hardware->l2->sets[i].tags[j] = 0;
			hardware->l2->sets[i].dirty[j] = 0;
		}
	}

	FILE *out = freopen("logs.txt", "w", stdout);

	DIR *dir;
	struct dirent *dirEntry;
	int numFiles = 0;

	dir = opendir("./InputFiles/");

	while ((dirEntry = readdir(dir)) != NULL)
	{
		if (dirEntry->d_type != DT_REG)
			continue;
		else
			numFiles++;
	}
	printf("Number of Input Files = %d\n", numFiles);

	rewinddir(dir);

	char *fileList[numFiles];
	int i = 0;

	while ((dirEntry = readdir(dir)) != NULL)
	{
		if (dirEntry->d_type != DT_REG)
			continue;
		else
		{
			char path[120] = "InputFiles/";
			fileList[i] = (char *)malloc(strlen(dirEntry->d_name) + strlen(path) + 1);
			strcat(path, dirEntry->d_name);
			strncpy(fileList[i], path, strlen(path));
			fileList[i][strlen(path)] = '\0';
			i++;
		}
	}
	if (closedir(dir) == -1)
	{
		perror("closedir");
		return 1;
	}

	for (int i = 0; i < numFiles; i++)
		printf("%s\n", fileList[i]);

	simulate(hardware, fileList, numFiles);

	fclose(out);
	return 0;
}
