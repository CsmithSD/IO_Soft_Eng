# compiler
CC = g++

# compiler options
CFLAGS = -std=c++11 -c -Wall

all: tester

tester: tester.o
	$(CC) -lm ProgramTester.o -o ProgramTester

tester.o: ProgramTester.cpp
	$(CC) $(CFLAGS) ProgramTester.cpp

clean:
	rm *.o ProgramTester
