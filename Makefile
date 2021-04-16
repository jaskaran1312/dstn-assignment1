build: exe
		./exe


exe: main.c init.c tlb.c l1cache.c victimcache.c l2cache.c 
		gcc -o exe main.c init.c tlb.c l2cache.c l1cache.c victimcache.c -ll


clean:
		rm -rf exe logs.txt
