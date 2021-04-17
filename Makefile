build: exe
		./exe


exe: main.c init.c tlb.c l1cache.c victimcache.c l2cache.c lru.c thrashing.c
		gcc -o exe main.c init.c tlb.c l2cache.c l1cache.c victimcache.c lru.c thrashing.c -ll


clean:
		rm -rf exe logs.txt
