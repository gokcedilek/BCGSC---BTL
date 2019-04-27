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
#include <future>
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
std::mutex m1;
std::mutex m2;
std::mutex m3;
std::mutex m4;
std::mutex m;

static void countMxs(unsigned thread_id, const BxtoMxs& bxtomxs, unsigned num_threads, MxtoCount& counts) {
	counts.clear(); //you need to re-count everytime
    assert(counts.empty());
	for (unsigned index = thread_id; index < bxtomxs.size(); index+=num_threads) {
		auto it = std::next(bxtomxs.begin(), index);
		const auto& mxs = it->second;
		for (const auto& mx : mxs) {
			{
				std::lock_guard<std::mutex> lock(m);
				if (counts.find(mx) == counts.end()) {
					counts[mx] = 1;
				} else {	
					++counts[mx];
				}
			}
		}
	}
}
			
static void print_count (const MxtoCount& counts) {
	for (auto it = counts.begin(); it != counts.end(); ++it){
		std::cout << it->first << " : " << it->second << '\n';
	}
}

//idea for multi-threading reading: read from the file with a single thread, but process the information that you've read with multiple threads

static void filterMxs(std::promise<int> rareMx, std::promise<int> freqMx, unsigned thread_id, BxtoMxs& bxtomxs, unsigned num_threads, MxtoCount& counts, unsigned C, tsl::robin_set<int>& rareMxs, tsl::robin_set<int>& freqMxs) {
	assert (!bxtomxs.empty() && !counts.empty());
	unsigned rare = 0;
	unsigned frequent = 0;
	for (unsigned index = thread_id; index < bxtomxs.size(); index+=num_threads) {
		auto it = std::next(bxtomxs.begin(), index);
		const auto& bx = it->first;
		auto& mxs = it->second;
		tsl::robin_set<int> validMxs;
		validMxs.reserve(mxs.size());
		for (const auto& mx : mxs) {
			unsigned count = counts[mx];
			if (count < 2) {
				//ASK: each thread has their own clean copy of rare and frequent -- I do NOT need to lock these operations, do I?
				{
					std::lock_guard<std::mutex> lock(m1);
					rareMxs.insert(mx);
					//++rare;
					//std::cout << "rare: " << bx << " : " << mx << '\n';
				}
			} else if (count >= C) {
				{
					std::lock_guard<std::mutex> lock(m2);
					freqMxs.insert(mx);
					//++frequent;
					//std::cout << "freq: " << bx << " : " << mx << '\n';
				}
			} else {
				validMxs.insert(mx); //each thread has their own validMxs -- so this insertion is not a problem
			}
		}
		bxtomxs[bx] = std::move(validMxs);
	}
	//each of the threads finished THEIR set of bxs & mxs -- now add their final result to our promise
	rareMx.set_value(rare);
	freqMx.set_value(frequent);
}

static void filterBarcodes(std::promise<int> toofew, std::promise<int> toomany, unsigned thread_id, BxtoMxs& bxtomxs, unsigned num_threads, unsigned n, unsigned N) {
	unsigned too_few = 0;
	unsigned too_many = 0;
	for (unsigned index = thread_id; index < bxtomxs.size(); index+=num_threads) {
		auto it = std::next(bxtomxs.begin(), index);
		//solution: either keep a boolean vector, or do the barcode filtering the last
		const auto& bx = it->first;
		auto& mxs = it->second;
		//if the bx will be removed, just clear all of its mxs -- do NOT remove the bx in here (do it main BEFORE writing to file! //
		if (mxs.size() < n) {
			++too_few; 
			bxtomxs.find(bx).value().clear();
		} else if (mxs.size() >= N) {
			++too_many;
			bxtomxs.find(bx).value().clear();
		}
	}
	toofew.set_value(too_few);
	toomany.set_value(too_many);
}

static std::vector<float> quantile (const MxtoCount& counts) {
	//in quantiles, we would want to exclude the elements that occur with a multiplicity less than "c" -- for now exclude singletons
	std::vector<int> values;
	values.reserve(counts.size());
	for(const auto& item : counts) {
		if (item.second != 1) {
			values.push_back(item.second);
		}
	}
	std::sort(values.begin(), values.end());
	std::vector<float> q = {0.25,0.5,0.75};
	std::vector<float> qs;
	qs.reserve(q.size());
	std::fesetround(FE_TONEAREST);
	for (const auto& p : q){
    	qs.push_back(values[size_t(std::nearbyint(p*(values.size()-1)))]);
    }
	return qs;
}

int main(){
	//read bxtomxs
	BxtoMxs bxtomxs =  {{"BX1", {1,2,3,4}}, {"BX2",{4,5,6,9}}, {"BX3",{0,1,6,7,8}}, {"BX4", {4,5,6,7,8, 10}}, {"BX5", {1,2,3,4,5}}, {"BX6", {1,2,3,4}}, {"BX7", {0,6,7,8}}, {"BX8", {1,2,3,14}}, {"BX9", {4,5,6,9}}, {"BX10", {0, 12, 14}}};
	MxtoCount counts;
	tsl::robin_set<int> rareMxs;
	tsl::robin_set<int> freqMxs;
	unsigned num_threads = 6;
	std::vector <std::thread> t(num_threads);
	for (unsigned i =0; i< num_threads; ++i) {
		t[i] = std::thread(&countMxs, i, std::ref(bxtomxs), num_threads, std::ref(counts));
	}
	for (auto& thr : t) {
		if (thr.joinable()) {
			thr.join();
		}
	}
	std::vector<float> q = quantile(counts);
	unsigned C = int(q[2] + 1.5 * (q[2] - q[0])); //call the quantile function here before filtering mxs
	std::cout << "quan: " << q[0] << ' ' << q[1] << ' ' << q[2] << '\n';
	//unsigned C = 5;
	unsigned rare = 0;
	unsigned frequent = 0;
	for (unsigned i =0; i< num_threads; ++i) {
		//std::promise<int> rare_promise;
		//std::future<int> rare_future = rare_promise.get_future();
		//std::promise<int> freq_promise;
        std::future<int> freq_future = freq_promise.get_future();
		t[i] = std::thread(&filterMxs, std::move(rare_promise), std::move(freq_promise), i, std::ref(bxtomxs), num_threads, std::ref(counts), C, std::ref(rareMxs), std::ref(freqMxs));
		rare += rare_future.get(); //no need to lock again because a single thread tries to add each time?
		frequent += freq_future.get();
	}
	for (auto& thr : t){
		if(thr.joinable()){
			thr.join();
		}
	}
	std::cout << "rare: " << rareMxs.size() << " / " << counts.size() << '\n';
	std::cout << "freq: " << freqMxs.size() << " / " << counts.size() << '\n';
	for(const auto& item : bxtomxs) {
		std::cout << item.first << ' ';
		for(const auto& mx : item.second) {
			std::cout << mx << ' ';
		}
		std::cout << '\n';
	}
	unsigned toofew =0;
	unsigned toomany =0;
	unsigned n = 3;
	unsigned N = 5;
	for (unsigned i =0; i< num_threads; ++i) {
		std::promise<int> toofewP;
		std::future<int> toofewF = toofewP.get_future();
		std::promise<int> toomanyP;
		std::future<int> toomanyF = toomanyP.get_future();
		t[i] = std::thread(&filterBarcodes, std::move(toofewP), std::move(toomanyP), i, std::ref(bxtomxs), num_threads, n, N);
		toofew += toofewF.get();
		toomany += toomanyF.get();
	}
	for (auto& thr : t){
		if(thr.joinable()){
			thr.join();
		}
	}
	std::cout << "too few: " << toofew << '\n';
	std::cout << "too many: " << toomany << '\n';
	//print_count(counts);*/
	for(const auto& item : bxtomxs) {
		std::cout << item.first << ' ';
		for(const auto& mx : item.second) {
			std::cout << mx << ' ';
		}
		std::cout << '\n';
	}
}
