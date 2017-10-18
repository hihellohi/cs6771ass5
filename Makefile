all: sortTester

sortTester : sortTester.o BucketSort.o old.o
	g++ -std=c++14 -Wall -Werror               -O2 -pthread -o sortTester sortTester.o BucketSort.o old.o

sortTester.o: sortTester.cpp BucketSort.h
	g++ -std=c++14 -Wall -Werror               -O2 -pthread -c sortTester.cpp

BucketSort.o : BucketSort.h BucketSort.cpp
	g++ -std=c++14 -Wall -Werror               -O2 -pthread -c BucketSort.cpp

old.o: old.cpp old.h
	g++ -std=c++14 -Wall -Werror               -O2 -pthread -c old.cpp

clean:
	rm *.o sortTester
