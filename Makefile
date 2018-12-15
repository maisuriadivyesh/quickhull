all:
	gcc quickHull.c -o submission

clean:
	rm -rf submission quickHull.o
