#include <algorithm>
#include <atomic>
#include <boost/range/algorithm.hpp>
#include <boost/range/adaptors.hpp>
#include <cassert>
#include <climits>
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
#include <omp.h>
#include <thread>
#include "tsl/robin_map.h"
#include "tsl/robin_set.h"

using Mx = uint64_t;
//using Mxs = tsl::robin_set<Mx>;
using Mxs = std::vector<unsigned>;
using Bx = std::string;
using BxtoMxs = std::unordered_map<std::string, std::vector<unsigned>>;
std::mutex m;
std::mutex m2;
void filter_write (BxtoMxs bxtomxs, std::string key){
//only process a given index?
	auto& bx = key;
	auto& mxs = bxtomxs[key];
	std::ostringstream oss;
	auto th_id = std::this_thread::get_id();
	oss << "In thread: " << th_id << '\n';
	oss << bx;
	char sep = '\t';
	for(const auto& mx : mxs){
		oss << sep << mx;
		sep = ' ';
	}
	oss << '\n';
	std::cout << oss.str();
}

void try_func (BxtoMxs::iterator& it){
	{
		std::lock_guard<std::mutex> lock(m);
		++it;
	}
} //doesn't work

void filter_write_2 (BxtoMxs& bxtomxs, BxtoMxs::iterator& it){
	//this function should filter singleton minimizers, repetitive minimizers, barcodes having too few or too many minimizers, and write all the remaining to a file
	auto& bx = it->first;
	auto& mxs = it->second;
	auto th_id = std::this_thread::get_id();
	//auto mxs = std::ref(it->second);
	Mxs not_singletons;
	not_singletons.reserve(mxs.size());
	//std::mutex m;
	for (const auto& mx : mxs){
		if(mx >= 3){
			not_singletons.push_back(mx);
		}
	}
	bxtomxs[bx] = std::move(not_singletons);
	auto& new_mxs = it->second; //can we update mxs?
	/*if(mxs.size() < 3){
	}
	else{*/
		std::ostringstream oss;
		oss << "Thread: " << th_id << '\n';
		oss << bx << '\n';
		for (const auto& mx : new_mxs){
			oss << mx << ' ';
		}
		oss << "\n";
		std::cout << oss.str();
	/*}*/
	std::lock_guard<std::mutex> lock(m);
	++it;
}

void func(BxtoMxs& bxtomxs){
	for(auto it = bxtomxs.begin(); it != bxtomxs.end(); ){
		const auto& bx = item->first;
		auto& mxs = it->second;
		Mxs not_singletons;
        not_singletons.reserve(mxs.size());

	}
}

int main(){
	int num_t = 4;
	threadPool t_pool;
	t_pool.start(num_t);
	/*for(auto it = bxtomxs.begin(); it != bxtomxs.end(); ++it){
		t_pool.start(num_t
	}*/
}

class threadPool{
	public:
		threadPool(int num_threads): {}

		void start(){
			
		}
}

int main(){
	int num_threads = 4;
    std::vector <std::thread> t(num_threads);
	BxtoMxs bxtomxs = {{"BX1", {1,2,3}}, {"BX2",{4,5}}, {"BX3",{6,7,8}}, {"BX4", {5,6,7,8}}, {"BX5", {1,2,3,4,5}}};
	auto start = bxtomxs.begin();
	BxtoMxs::iterator it;
	for(unsigned i=0; i< num_threads; ++i){
		// some data structure to store chunk in
		BxtoMxs min;
		for(it = start; it != std::next(start, size/num_threads); ++it){
			// add things into chunk
			min.insert({it->first, it->second});
		}
		// give chunk to thread (function in thread iterates through ur chunk)
		t[i] = std::thread(&func, bxtomxs);
		// update iterator
		start = it;
	}
	for (auto& thr : t){
		if(thr.joinable()){
	        thr.join();
		}
	}
}

int main(){
	int num_threads = 4;
	std::vector <std::thread> t(num_threads);
	int i=0;
	for(auto it = bxtomxs.begin(); it != bxtomxs.end(); ++it){
		//don't pass "it" as a reference!!! just as a value!!!
		
	}
}

int main() {
	BxtoMxs bxtomxs = {{"BX1", {1,2,3}}, {"BX2",{4,5}}, {"BX3",{6,7,8}}, {"BX4", {5,6,7,8}}, {"BX5", {1,2,3,4,5}}};
	int num_threads = 4;
	std::vector <std::thread> t(num_threads);
	int i=0;
	int size = bxtomxs.size();
	//std::atomic<BxtoMxs::iterator> it;
	//std::cout << size << '\n';
	//std::mutex m;
	
	for (auto it = boost::begin(0); it != bxtomxs[3].end(); ){
		std::cout << it->first << '\n';
		//incrementing in the outer loop didn't work
		/*{
			std::lock_guard<std::mutex> lock(m2);
			t[i%num_threads] = std::thread(&try_func, std::ref(it));
			std::cout << "In main: " << it->first << '\n';
		}*/
		/*t[i%num_threads] = std::thread(&filter_write_2,std::ref(bxtomxs),std::ref(it));
		if ( (num_threads <= size) && (i%num_threads == num_threads -1) ){
			for (auto& thr : t){
				if(thr.joinable()){
					thr.join();
					//std::for_each(t.begin(), t.end(), std::mem_fn(&std::thread::join));
				}
			}
		}
		else if( (num_threads <= size) && (i == size -1) ){
			for (auto& thr : t){
				if(thr.joinable()){
					thr.join();
				}
			}
		}
		else if( (num_threads > size) && (i == size -1) ){
			for (auto& thr : t){
				if(thr.joinable()){
					thr.join();
				}
			}
		}
		++i;*/
	}
}
/*
int main (){
	BxtoMxs bxtomxs = {{"BX1", {1,2,3}}, {"BX2",{4,5}}, {"BX3",{6,7,8}}, {"BX4", {5,6,7,8}}, {"BX5",     {1,2,3,4,5}}};
	//int num_threads = 16;
	//std::vector <std::thread> t(num_threads);
	//int i=0;
	//int size = bxtomxs.size();
	for(auto it = bxtomxs.begin(); it != bxtomxs.end(); ){
		filter_write_2(bxtomxs, it);
	}
	//after doing the work in the helper write:
	std::cout << "finished helper, data structure in main: " << '\n';
	for(auto it = bxtomxs.begin(); it!=bxtomxs.end(); ++it){
		std::cout << it->first << '\n';
		for (const auto& mx : it->second){
			std::cout << mx << ' ';
		}
		std::cout << std::endl;
	}
}
*/

//int num_iterations = bxtomxs.size();
	/*for (int i=0; i< num_iterations; ++i){
		std::cout << "In thread: " << i%num_threads << '\n'; 
		t[i%num_threads] = std::thread(&filter_write,bxtomxs,i);
		//t.push_back(std::thread(&filter_write,bxtomxs,i%num_threads));
	}*/

//#pragma omp parallel num_threads(3)
	
//	for (auto it = bxtomxs.begin(); it != bxtomxs.end(); ) {
//		auto& bx = it->first;
//		auto& mxs = it->second;
//		int tid = omp_get_thread_num();
//			std::cout << "Before removal: Thread}
	
//if we try to parallelize reading the same data structure, doesn't really work the way we want it to.
	//so, read each mx one by one, and read their entries in a parallel region
	/*for (const auto& item : bxtomxs){
		const auto& mxs = item.second;
		#pragma omp parallel
		#pragma omp critical
		//int tid=omp_get_thread_num();
		//std::cout << tid << ' ';
		for (const auto& mx: mxs){
			std::cout << mx << ' ';
		}
		std::cout<< '\n';
	}*/
	/*std::vector<std::string> files = {"banana", "berry", "apple", "orange"};
	std::string f;
	#pragma omp parallel
	{
		#pragma omp atomic
	int t= omp_get_thread_num();
	for (std::string f: files) {
		//#pragma omp critical
		std::cout << "file: " << f << " thread: " << t << '\n';
	}
	}*/
