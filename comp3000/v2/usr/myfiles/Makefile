all: setencrypt.o
	cc -o setencrypt setencrypt.o

setencrypt.o: setencrypt.c
	cc -c setencrypt.c

clean:
	rm -f *.o setencrypt
