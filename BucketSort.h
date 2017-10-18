#ifndef BUCKET_SORT_H
#define BUCKET_SORT_H

#include <vector>
#include <mutex>
#include <queue>

struct BucketSort {
	
	// vector of numbers
	std::vector<unsigned int> numbersToSort;

	void sort(unsigned int numCores);
};

#endif /* BUCKET_SORT_H */
