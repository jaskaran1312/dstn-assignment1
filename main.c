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
#include "mainMemory.h"

void fetchData(long pa, struct Hardware *hardware)
{
	//TODO statistic variables

	//First try L1 cache
	if (!fetchL1Cache(pa, hardware))
		return; //Hit in L1

	//Miss in L1; try Victim cache
	if (!fetchVictimCache(pa, hardware))
	{
		//Hit in Victim
		updateL1Cache(pa, hardware, 1);
		return;
	}

	//Miss in Victim; try L2
	if (!fetchL2Cache(pa, hardware))
	{
		//Hit in L2
		updateL1Cache(pa, hardware, 0);
		return;
	}

	//Miss in L2
}

int main()
{

	struct Hardware *hardware = (struct Hardware *)malloc(sizeof(struct Hardware));

	init(hardware);

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
