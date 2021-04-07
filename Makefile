build: exe
		./exe

exe: main.c l2cache.c 
		gcc -o exe main.c l2cache.c -ll

clean:
		rm -f exe