#include <iostream>
#include <random>
#include <thread>
#include <cassert>
#include <iterator>

#include "BucketSort.h"
#include "old.h"

int main() {
	
	unsigned int totalNumbers =	50000000;
	//unsigned int printIndex =	259;
	
	// use totalNumbers required as the seed for the random
	// number generator. 
	std::mt19937 mt(totalNumbers);
	std::uniform_int_distribution<unsigned int> dist(1, std::numeric_limits<unsigned int>::max());

	// create a sort object
	BucketSort pbs;
	//BucketSort2 pbs2;

	std::cout <<  "generating" << std::endl;

	// insert random numbers into the sort object
	for (unsigned int i=0; i < totalNumbers; ++i) {
		auto j = dist(mt);
		pbs.numbersToSort.push_back(j);
		//pbs2.numbersToSort.push_back(j);
	} 
	std::cout <<  "generating done" << std::endl;
	
	// call sort giving the number of cores available.
        const unsigned int numCores = std::thread::hardware_concurrency();
        pbs.sort(numCores);
        //pbs2.sort(numCores);

		//std::copy(pbs.numbersToSort.cbegin(), pbs.numbersToSort.cend(), std::ostream_iterator<unsigned>(std::cout, " "));
		//std::cout << std::endl;
		//std::copy(pbs2.numbersToSort.cbegin(), pbs2.numbersToSort.cend(), std::ostream_iterator<unsigned>(std::cout, " "));

        std::cout << "number of cores used: " << numCores << std::endl;
	//assert(std::equal(pbs.numbersToSort.begin(), pbs.numbersToSort.end(), pbs2.numbersToSort.begin()));
	
	// print certain values from the buckets
	//std::cout << "Demonstrating that all the numbers that start with 1 come first" << std::endl;
	//std::cout << pbs.numbersToSort[0] << " " << pbs.numbersToSort[printIndex - 10000]
	//	<< " " << pbs.numbersToSort[printIndex] << " " << pbs.numbersToSort[pbs.numbersToSort.size() - 1] 
	//	<< std::endl;
	
}
