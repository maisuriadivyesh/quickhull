all:
	gcc quickHull.c -o quickHull

clean:
	rm -rf quickHull quickHull.o
