#include <stdio.h>
#include <stdlib.h>
#include "hardware.h"
#include "mainMemory.h"
#include "l2cache.h"
#include "l1cache.h"

int fetchMainMemory(long la, long base, struct Hardware *hardware, struct Process *process)
{
	//Returns 0 for Page Fault and 1 for Data Hit
	int pdOffset = (la >> 17);
	int ptOffset = (la >> 9) & (0xff);
	int dataOffset = la & (0x1ff);

	//Does the Page Directory Frame belong to the process?
	if (checkFrameTable(base, process))
	{																		//Yes
		short ptpa = hardware->mainMemory->frames[base]->entries[pdOffset]; //physical address of page table;

		//Does the Page TABLE Frame belong to the process?
		if (checkFrameTable(ptpa, process))
		{
			//Yes
			short pagepa = hardware->mainMemory->frames[ptpa]->entries[dataOffset]; //physical address of page table;
			// Does the Page belong to the process?
			if (checkFrameTable(pagepa, process))
			{
				// Data Found
				updateL2Cache(pagepa, hardware);
				updateL1Cache(pagepa, hardware, 0);
				return 1;
			}
			else
			{
				short pa = allocateNewFrame();
				hardware->mainMemory->frames[ptpa]->entries[dataOffset] = pa;
				return 0;
			}
		}
		else
		{ // Page Fault for Page Table
			short pa = allocateNewFrame();
			hardware->mainMemory->frames[ptpa]->entries[ptOffset] = pa;
			return 0;
		}
	}
	else
	{ //Page Fault for Page Directory
		short pa = allocateNewFrame();
		hardware->mainMemory->frames[base]->entries[pdOffset] = pa;
		return 0;
	}
}
