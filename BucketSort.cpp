#include "BucketSort.h"

#include <algorithm>
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <utility>
#include <stack>
#include <array>
#include <mutex>
#include <condition_variable>
#include <thread>

static const int min_interval = 9000;
static const unsigned min_init_interval = 9000;

struct job {
	job () = default;
	job (std::vector<char*>::iterator b, std::vector<char*>::iterator e, size_t i) : begin{b}, end{e}, index{i} {}

	std::vector<char*>::iterator begin;
	std::vector<char*>::iterator end;
	size_t index;
};

void populate(std::vector<char*> strings, std::vector<unsigned> ints, unsigned int numCores){

	auto calc = [&](unsigned int id, 
			std::vector<unsigned>::const_iterator start,
			std::vector<unsigned>::const_iterator end,
			std::vector<char*>::iterator out){

		for(; start != end; start++, out++){
			int i;
			int cur = *start;
			for(i = 0; i < 10 && cur; i++){
				(*out)[i] = '0' + (cur % 10);
				cur /= 10;
			}
			(*out)[i--] = 0;
			for(int j = 0; j < i; j++, i--) {
				std::swap((*out)[j], (*out)[i]);
			}
		}
		//std::transform(start, end, out, 
		//		static_cast<char*(*)(unsigned)>(std::to_string));
	};

	std::vector<std::thread> threads;
	auto numit = ints.cbegin();
	auto stringit = strings.begin();
	int jobsize = std::max(min_init_interval, static_cast<unsigned>((ints.size() + numCores - 1) / numCores));

	for(unsigned i = 1; i < numCores; i++) {
		if(ints.cend() - numit < jobsize * 2) {
			break;
		}
		threads.emplace_back(calc, i, numit, numit + jobsize, stringit);
		numit += jobsize;
		stringit += jobsize;
	}

	calc(0, numit, ints.cend(), stringit);

	for(auto &t: threads) {
		t.join();
	}

}

void BucketSort::sort(unsigned int numCores) {
	std::vector<char*> strings(numbersToSort.size());
	for(auto &string : strings) {
		string = new char[11];
	}
	populate(strings, numbersToSort, numCores);

	unsigned int running = numCores;

	std::mutex m;
	std::condition_variable cv;

	std::stack<job> jobs;
	jobs.emplace(strings.begin(), strings.end(), 0u);

	auto calc = [&](unsigned int id){

		std::array<std::vector<char*>, 10> buckets;
		std::vector<char*> finished;

		job cur;
		bool hasjob = false;
		while(true) {
			if(!hasjob) {
				//aquire job
				std::unique_lock<std::mutex> lock(m);
				running--;

				cv.wait(lock, [&jobs, &running] {
						return !jobs.empty() || !running;
					});

				if(jobs.empty()) {
					break;
				}

				cur = jobs.top();
				jobs.pop();
				running++;
			}

			if(cur.end - cur.begin < min_interval) {
				//if job small enough, just solve single threadedly
				std::sort(cur.begin, cur.end);
				std::transform(cur.begin, cur.end, numbersToSort.begin() + (cur.begin - strings.begin()),
						[](auto s) { return std::stoul(s);});
				hasjob = false;
			}
			else {
				// n partition
				for(auto it = cur.begin; it != cur.end; it++) {
					if(!(*it)[cur.index]) {
						finished.push_back(*it);
					} else {
						buckets[(*it)[cur.index] - '0'].push_back(*it);
					}
				}

				auto last = std::move(finished.begin(), finished.end(), cur.begin);
				finished.clear();

				if(last != cur.end) {
					for(auto &bucket : buckets) {
						auto now = std::move(bucket.begin(), bucket.end(), last);
						if(now != last) {
							bucket.clear();
							if(now == cur.end) {
								cur.begin = last;
								cur.end = now;
								cur.index++;
								hasjob = true;
								break;
							}

							std::lock_guard<std::mutex> lock(m);
							jobs.emplace(last, now, cur.index + 1);
							cv.notify_one();

							last = now;
						}
					}
				}
				else {
					hasjob = false;
				}
			}
		}

		cv.notify_all();
	};

	std::vector<std::thread> threads;

	for(unsigned i = 1; i < numCores; i++) {
		threads.emplace_back(calc, i);
	}

	calc(0);

	for(auto &t: threads) {
		t.join();
	}
	for(auto &string : strings) {
		delete[] string;
	}
}
