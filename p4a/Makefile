.PHONY: all
all : libcrawler.so file_tester web_tester

file_tester : file_tester.c libcrawler.so
	gcc -g -L. -lcrawler -lpthread file_tester.c -Wall -Werror -o file_tester

web_tester : web_tester.c cs537.c libcrawler.so
	gcc -g -L. -lcrawler -lpthread web_tester.c cs537.c -Wall -Werror -o web_tester

libcrawler.so : crawler.c linkedlist.o
	gcc -g -fpic -c crawler.c -Wall -Werror -o crawler.o
	gcc -g -shared -o libcrawler.so crawler.o linkedlist.o

linkedlist.o : linkedlist.c linkedlist.h
	gcc -g -fpic -c linkedlist.c -Wall -Werror -o linkedlist.o

.PHONY: clean
clean :
	rm -f file_tester web_tester libcrawler.so *.o *~
