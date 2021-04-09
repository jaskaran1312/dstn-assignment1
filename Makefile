build: exe
		./exe

exe: main.c l2cache.c l1cache.c 
		gcc -o exe main.c l2cache.c l1cache.c -ll

clean:
		rm -f exe
