build: exe
		./exe


exe: main.c init.c l1cache.c victimcache.c l2cache.c 
		gcc -o exe main.c init.c l2cache.c l1cache.c victimcache.c -ll


clean:
		rm -f exe
