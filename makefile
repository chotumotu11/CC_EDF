all: main.c 
	gcc -o final main.c  -Wall -I.

clean:
	rm -rf *.o	