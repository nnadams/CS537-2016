.PHONY: all
all: fscheck

fscheck: fscheck.o
	gcc -g -Wall -Werror fscheck.o -o fscheck

fscheck.o: fscheck.c
	gcc -g -Wall -Werror -c fscheck.c -o fscheck.o

.PHONY: clean
clean:
	rm -rf fscheck *.o *~ .fuse*
