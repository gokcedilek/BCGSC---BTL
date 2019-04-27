#include "tsl/robin_map.h"
#include "tsl/robin_set.h"
#include <algorithm>
#include <cassert>
#include <cfenv>
#include <chrono>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>
#include <getopt.h>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set> 
#include <vector>

using Mx = uint64_t;
using Mxs = tsl::robin_set<Mx>;
using Bx = std::string;
using BxtoMxs = tsl::robin_map<Bx, tsl::robin_set<int>>;
using MxtoCount = tsl::robin_map<int, unsigned>;

/*static void func(BxtoMxs& subset){
	std::ostringstream oss;
	oss << "in func with  " << std::this_thread::get_id() << '\n';
	for(auto it = subset.begin(); it != subset.end(); ++it){
		oss << it->first << ' '; 
		for(auto& i : it->second){
			oss << i << ' ';
		}
		oss << "\n\n";
	}
	std::cout << oss.str();
}*/
/*
static void func_2(BxtoMxs subset){
	
}*/

/*int main(int argc, char *argv[]){
	BxtoMxs bxtomxs =  {{"BX1", {1,2,3}}, {"BX2",{4,5}}, {"BX3",{6,7,8}}, {"BX4", {5,6,7,8}}, {"BX5", {1,2,3,4,5}}, {"BX6", {1,2,3,4}}};
	int num_threads = 2;
	int size = bxtomxs.size();
	int increment = size / num_threads;
	std::vector <std::thread> t(num_threads);
	for (int i=0; i< size; i+=increment){
		int start = i;
		int end = i + increment -1;
		auto it = bxtomxs.begin() + i;
		auto end = it + increment;
		//bxtomxs[start:end] --> send to t[i/increment]
		std::cout << "New\n";
	}
}*/

/*static void count(const BxtoMxs& subset, MxtoCount& counts){
	//std::cout << "one\n";
	{
		std::mutex m;
		std::lock_guard<std::mutex> lock(m);
		//std::cout << "two\n";
		for (const auto& item : subset)
		{
			const auto& mxs = item.second;
			for (const auto& mx : mxs) {
				if (counts.find(mx) == counts.end()) {
						counts[mx] = 1;
				} else {
						++counts[mx];
				}
			}
			std::cout << "Counted subset: " << item.first << '\n';
		}
		//std::cout << "three\n";
	}
	//std::cout << "four\n";
}*/

static void print (std::vector <BxtoMxs> mins){
	for (unsigned i=0; i< mins.size(); ++i) {
		std::cout << "New subset:\n";
		for (auto it = mins[i].begin(); it != mins[i].end(); ++it) {
			std::cout << it->first << ' ';
			for (auto mx : it->second) {
				std::cout << mx << ' ';
			}
			std::cout << '\n';
		}
		std::cout << '\n';
	}
}

static void single_count(const BxtoMxs& bxtomxs, MxtoCount& counts){
	if (!counts.empty()) {
		counts.clear();
	}
	for (const auto& item : bxtomxs)
	{
		const auto& mxs = item.second;
		for (const auto& mx : mxs) {
				if (counts.find(mx) == counts.end()) {
						counts[mx] = 1;
				} else {
						++counts[mx];
				}
		}
	}
	std::cout << "finished counts\n";
}

static void generic(BxtoMxs& subset, MxtoCount& counts){
	//this func will call other functions on subsets of BxtoMxs and on the shared variable counts
	//std::ostringstream oss;
	//oss << "in generic with " << std::this_thread::get_id() << '\n';
	for (auto it = subset.begin(); it != subset.end(); ++it){
		const auto& mxs = it->second;
		tsl::robin_set<int> news;
		for (const auto& mx : mxs){
			if(counts[mx] >= 3) {
				news.insert(mx);
			}
		}
		subset[it->first] = std::move(news);
	}
	//subset.clear();
	//std::cout << subset.size() << '\n';
	/*for(auto item : subset){
		oss << item.first << ' ';
	}*/
	//oss << '\n';
	//std::cout << oss.str();
	//count(subset, counts);
}

int main(){
	//read bxtomxs
	//create the counts data structure
	BxtoMxs bxtomxs =  {{"BX1", {1,2,3}}, {"BX2",{4,5}}, {"BX3",{6,7,8}}, {"BX4", {5,6,7,8}}, {"BX5", {1,2,3,4,5}}, {"BX6", {1,2,3,4}}};
	MxtoCount counts;
	single_count(bxtomxs, counts); //--> this could either be done here in a single-threaded way, or this could be executed with a mutex lock for each thread executing on a subset of BxtoMxs
	unsigned num_threads = 2;
	int size = bxtomxs.size(); //after reading : initial size
    std::vector <std::thread> t(num_threads);
	auto start = bxtomxs.begin();
    BxtoMxs::iterator it;
	std::vector <BxtoMxs> mins(num_threads);
	for(unsigned i=0; i< num_threads; ++i){
		for(it = start; it != std::next(start, size/num_threads); ++it){
			mins[i].insert({it->first, it->second});
		}
		t[i] = std::thread(&generic, std::ref(mins[i]), std::ref(counts));
		//there can be a 'general' function that calls all the other functions that each subset will go through
		start = it;
	}
	for (auto& thr : t){
		if(thr.joinable()){
			thr.join();
		}
	}
	std::cout << "joined\n";
	print(mins);
	//single_count(bxtomxs, counts);

}


