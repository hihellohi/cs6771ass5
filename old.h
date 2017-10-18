#ifndef BUCKET_SORT2_H
#define BUCKET_SORT2_H

#include <vector>

struct BucketSort2 {
	
	// vector of numbers
	std::vector<unsigned int> numbersToSort;

	void sort(unsigned int numCores);
};


#endif /* BUCKET_SORT_H */
