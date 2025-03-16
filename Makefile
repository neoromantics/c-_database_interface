CC=g++
CFLAGS=-O3
EXTRAFLAGS=-lpqxx -lpq

all: test

test: main.o exerciser.o query_funcs.o DatabaseManager.o ColorTable.o PlayerTable.o StateTable.o
	$(CC) $(CFLAGS) -o test main.o exerciser.o query_funcs.o DatabaseManager.o ColorTable.o PlayerTable.o StateTable.o $(EXTRAFLAGS)

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

exerciser.o: exerciser.cpp exerciser.h
	$(CC) $(CFLAGS) -c exerciser.cpp

query_funcs.o: query_funcs.cpp query_funcs.h
	$(CC) $(CFLAGS) -c query_funcs.cpp

DatabaseManager.o: DatabaseManager.cpp DatabaseManager.h
	$(CC) $(CFLAGS) -c DatabaseManager.cpp

ColorTable.o: ColorTable.cpp ColorTable.h
	$(CC) $(CFLAGS) -c ColorTable.cpp

PlayerTable.o: PlayerTable.cpp PlayerTable.h
	$(CC) $(CFLAGS) -c PlayerTable.cpp

StateTable.o: StateTable.cpp StateTable.h
	$(CC) $(CFLAGS) -c StateTable.cpp

clean:
	rm -f *~ *.o test

clobber:
	rm -f *~ *.o
