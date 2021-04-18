debug: exe
		./exe -DDEBUG

build: exe
		./exe


exe: main.c init.c tlb.c l1cache.c victimcache.c l2cache.c lru.c thrashing.c
		gcc -o exe main.c init.c tlb.c l2cache.c l1cache.c victimcache.c lru.c thrashing.c frametable.c mainMemory.c segmentTable.c -ll


clean:
		rm -rf exe logs.txt result.txt
