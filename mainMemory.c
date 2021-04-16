#include<stdio.h>
#include<stdlib.h>
#include "hardware.h"

long fetchMainMemory (long la, long base, struct Hardware *hardware, struct Process *process) {
    	int pdOffset = (la >> 17);
    	int ptOffset = (la >> 9) & (0xff);
    	int dataOffset = la &(0x1ff);

	if(checkFrameTable(base, process)){//Does the base (PA) belong to the process? 	
		long ptpa = hardware->mainMemory[base][pdOffset]; //physical address of page table;

	}
	else{
		
	}



}
