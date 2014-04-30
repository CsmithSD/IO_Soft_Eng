CC = g++
CFLAGS  = -Wall -fprofile-arcs -ftest-coverage -pg

FILES = grade.cpp
EX = Grade

$(EX):
	$(CC) $(CFLAGS) -o $(EX) $(FILES)

clean:
	$(RM) $(EX)
