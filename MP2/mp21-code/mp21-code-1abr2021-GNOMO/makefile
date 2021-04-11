all: s c

s: server.o lib.o delay.c delay.h
	gcc -Wall -DDELAY=0 -o s delay.c lib.o server.o -pthread

c: client.c client.h
	gcc -Wall -lm -o c client.c -pthread -lm

# server.o: server.c
# 	gcc -Wall -c -o server.o server.c

# lib.o: lib.c lib.h
# 	gcc -Wall -c -o lib.o lib.c

clean:
	rm -f s c
