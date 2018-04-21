all: test1 test2

test1 :  exercise1-1.o
	gcc -Wall -g -std=gnu99 -o test1 $^

test2 :  exercise1-2.o
	gcc -Wall -g -std=gnu99 -o test2 $^

%.o : %.c
	gcc -Wall -g -c $^

clean : 
	rm -f *.o test1 test2