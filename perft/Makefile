CC = cc

CFLAGS = -Wall -Wno-parentheses
OPTFLAGS = -Ofast -march=native -DNDEBUG -fno-plt

all: main

perf: CFLAGS += $(OPTFLAGS)
perf: main

pgo: 
	$(CC) $(CFLAGS) $(OPTFLAGS) -fwhole-program -fprofile-generate main.c
	./a.out --pgo
	$(CC) $(CFLAGS) $(OPTFLAGS) -fwhole-program -fprofile-use main.c
	rm *.gcda

main: main.c
	$(CC) $(CFLAGS) main.c

clean:
	rm -f a.out *.gcda
