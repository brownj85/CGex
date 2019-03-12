CC=gcc
CFLAGS= -g -std=c99 -DDEBUG

%.o: %.c  %.h 
	$(CC) $(CFLAGS) -c $< -o $@

test: fsm.o util.o array_list/array_list.o regex.o regex_const.o test.c
	$(CC) $(CFLAGS) $^ -o $@

clean: 
	rm *.o test
