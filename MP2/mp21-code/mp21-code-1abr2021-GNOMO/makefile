all: s c

#s: server.o lib.o delay.c delay.h
#	gcc -Wall  -o s delay.c lib.o server.o -pthread

lib.o: lib.c lib.h delay.h
	gcc -Wall -c -o lib.o lib.c

s: server.c server.h lib.o delay.c delay.h
	gcc -Wall -lm -o s delay.c lib.o server.c -pthread -lm

c: client.c common.h
	gcc -Wall -lm -o c client.c -pthread -lm

# server.o: server.c
# 	gcc -Wall -c -o server.o server.c

# lib.o: lib.c lib.h
# 	gcc -Wall -c -o lib.o lib.c

clean:
	rm -f s c
