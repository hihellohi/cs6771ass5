#include "BucketSort.h"

#include <algorithm>
#include <cmath>
#include <vector>
#include <string>
#include <utility>
#include <queue>
#include <array>
#include <cassert>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>

static const int min_interval = 10000;

struct job {
	job () = default;
	job (std::vector<std::string>::iterator b, std::vector<std::string>::iterator e, size_t i) : begin{b}, end{e}, index{i} {}

	std::vector<std::string>::iterator begin;
	std::vector<std::string>::iterator end;
	size_t index;
};

std::vector<std::string> getstrings(std::vector<unsigned int> nums, unsigned int numCores){
	std::vector<std::string> strings(nums.size());
	std::vector<std::thread> threads;
	std::mutex m;
	auto it = nums.begin();

	auto fill = [&](unsigned i){
		while(true) {
			std::vector<unsigned int>::iterator begin;
			std::vector<unsigned int>::iterator end;
			{
				std::lock_guard<std::mutex> lock(m);	
				begin = it;
				if(it == nums.end()){
					break;
				}
				else if(nums.end() - it < min_interval) {
					end = it = nums.end();
				}
				else {
					end = it += min_interval;
				}
			}
			auto outbegin = strings.begin() + (begin - nums.begin());
			std::transform(begin, end, outbegin, static_cast<std::string(*)(unsigned long)>(std::to_string));
		}
	};
	
	for(unsigned i = 1; i < numCores; i++) {
		threads.emplace_back(fill, i);
	}

	fill(0);

	for(auto &t: threads) {
		t.join();
	}
	std::cout <<  "starting..." << std::endl;
	return strings;
}

void BucketSort::sort(unsigned int numCores) {
	auto strings = getstrings(numbersToSort, numCores);
	std::mutex m;

	unsigned int running = numCores;

	std::condition_variable cv;

	std::queue<job> jobs;
	jobs.emplace(strings.begin(), strings.end(), 0u);

	auto calc = [&](unsigned int id){

		std::array<std::vector<std::string>, 10> buckets;
		std::vector<std::string> finished;

		while(true) {
			job cur;
			{
				std::unique_lock<std::mutex> lock(m);
				running--;

				cv.wait(lock, [&jobs, &running] {
						return !jobs.empty() || !running;
					});

				if(jobs.empty()) {
					break;
				}

				cur = jobs.front();
				jobs.pop();
				running++;
			}

			if(cur.end - cur.begin < 1000) {
				std::sort(cur.begin, cur.end);
				std::transform(cur.begin, cur.end, numbersToSort.begin() + (cur.begin - strings.begin()), [](auto s) {return std::stoul(s);});
			}
			else {
				// rainbow sort
				for(auto it = cur.begin; it != cur.end; it++) {
					if(it->size() <= cur.index) {
						finished.push_back(std::move(*it));
					} else {
						buckets[it->at(cur.index) - '0'].push_back(std::move(*it));
					}
				}

				auto last = std::move(finished.begin(), finished.end(), cur.begin);
				finished.clear();

				for(auto &bucket : buckets) {
					if(last == cur.end) {
						break;
					}

					auto now = std::move(bucket.begin(), bucket.end(), last);
					if(now - last) {
						std::lock_guard<std::mutex> lock(m);
						jobs.emplace(last, now, cur.index + 1);
						cv.notify_one();
					}
					last = now;
					bucket.clear();
				}
			}
		}

		cv.notify_all();
	};


	std::cout <<  "starting..." << std::endl;
	std::vector<std::thread> threads;
	for(unsigned i = 1; i < numCores; i++) {
		threads.emplace_back(calc, i);
	}

	calc(0);

	for(auto &t: threads) {
		t.join();
	}
}
