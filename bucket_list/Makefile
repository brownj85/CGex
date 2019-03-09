CC=gcc
CFLAGS= -g -Wall -std=c99

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@



test: ../util.o bucket_list.o test.c 
	$(CC) $(CFLAGS) $^ -o $@

clean: 
	rm *.o test
