all: sortTester benchmark

sortTester : sortTester.o BucketSort.o old.o
	g++ -std=c++14 -Wall -Werror -O2 -pthread -g -o sortTester sortTester.o BucketSort.o old.o

sortTester.o: sortTester.cpp BucketSort.h
	g++ -std=c++14 -Wall -Werror -O2 -pthread -g -c sortTester.cpp

benchmark : benchmark.o BucketSort.o
	g++ -std=c++14 -Wall -Werror -O2 -pthread -g -o benchmark benchmark.o BucketSort.o

benchmark.o: benchmark.cpp BucketSort.h
	g++ -std=c++14 -Wall -Werror -O2 -pthread -g -c benchmark.cpp

BucketSort.o : BucketSort.h BucketSort.cpp
	g++ -std=c++14 -Wall -Werror -O2 -pthread -g -c BucketSort.cpp

old.o : old.h old.cpp
	g++ -std=c++14 -Wall -Werror -O2 -pthread -g -c old.cpp

clean:
	rm *.o sortTester benchmark
