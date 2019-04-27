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

static std::chrono::time_point<std::chrono::steady_clock> t0; //NOLINT(cert-err58-cpp)

static inline void assert_good(const std::ios& stream, const std::string& path)
{
	if (!stream.good()) {
		std::cerr << "error: " << strerror(errno) << ": " << path << '\n';
		exit(EXIT_FAILURE);
	}
}

static void printErrorMsg(const std::string& progname, const std::string& msg)
{
	std::cerr << progname << ": " << msg << "\nTry 'physlr-filter-barcodes-minimizers --help' for more information.\n";
}

static void printUsage(const std::string& progname)
{
	std::cout << "Usage:  " << progname << "  -n n -N N -c c -C C -t [-s] [-o file] file...\n\n"
	"  -s         silent; disable verbose output\n"
	"  -o file    write output to file, default is stdout\n"
	"  -n         minimum number of minimizers per barcode\n"
	"  -N         maximum number of minimizers per barcode\n"
	"  -c         minimum minimizer multiplicity\n"
	"  -C         maximum minimizer multiplicity\n"
	"  -t         number of threads\n"
	"  --help     display this help and exit\n"
	"  file       space separated list of FASTQ files\n";
}

using Mx = uint64_t;
using Mxs = tsl::robin_set<Mx>;
using Bx = std::string;
using BxtoMxs = tsl::robin_map<Bx, Mxs>;
using MxtoCount = tsl::robin_map<Mx, unsigned>;
std::mutex m1;
std::mutex m2;
std::mutex m3;
std::mutex m4;
std::mutex m5;

static void readMxs(std::istream &is, const std::string &ipath, bool silent, BxtoMxs& bxtomxs) {
	if (is.peek() == std::ifstream::traits_type::eof()) {
		std::cerr << "physlr-filter-barcodes-minimizers: error: Empty input file: " << ipath << '\n';
		exit(EXIT_FAILURE);
	}
	assert(bxtomxs.empty());
	Bx bx;
	std::string mx_string;
	std::string mx_line;
	std::istringstream iss;
	while ((is >> bx) && (getline(is, mx_line))) {
		iss.clear();
		iss.str(mx_line);
		auto& mxs = bxtomxs[bx];
		while (iss >> mx_string) {
			Mx mx = strtoull(mx_string.c_str(), nullptr, 0);
			mxs.insert(mx); 
		}
	}
	if (!silent) {
		auto t = std::chrono::steady_clock::now();
		auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(t - t0);
		std::cerr << "Time after reading minimizers (ms): " << diff.count() << '\n';
		std::cerr << "Read " << bxtomxs.size() << " barcodes." << "\n\n";
	}
}

static void writeMxs(BxtoMxs bxtomxs, std::ostream& os, const std::string& opath, bool silent) {
	unsigned wrote = 0;
	for (const auto& item : bxtomxs) {
		const auto& bx = item.first;
		const auto& mxs = item.second;
		if (!mxs.empty()) {
			++wrote;
			os << bx;
			char sep = '\t';
			for (const auto& mx : mxs) {
				os << sep << mx;
				sep = ' ';
			}
			os << '\n';
			assert_good(os, opath);
		}
	}
	auto t = std::chrono::steady_clock::now();
	auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(t - t0);
	if (!silent) {
		std::cerr << "Time after writing minimizers (ms): " << diff.count() << '\n';
		std::cerr << "Wrote " << wrote << " barcodes." << "\n\n";
	}
}

static void countMxs(unsigned thread_id, const BxtoMxs& bxtomxs, unsigned num_threads, MxtoCount& counts) {
	counts.clear(); 
	assert(counts.empty());
	for (unsigned index = thread_id; index < bxtomxs.size(); index+=num_threads) {
		auto it = std::next(bxtomxs.begin(), index);
		{
			std::lock_guard<std::mutex> lock(m1);
			const auto& mxs = it->second;
			for (const auto& mx : mxs) {
				if (counts.find(mx) == counts.end()) {
					counts[mx] = 1;
				} else {
					++counts[mx];
				}
			}
			//std::ostringstream oss;
	        //for (const auto& item : mxs) {
			//	std::cerr << std::this_thread::get_id() << " : " << it->first << " : " << item << " : " << counts[item] << '\n';
         	//}
			//std::cerr << "\n\n";
         	//oss << "\n\n";
         	//std::cerr << oss.str();
		}
	}
}
	

/*static void printCounts(const MxtoCount& counts) {
	for (const auto& item : counts) {
		std::cerr << item.first << " : " << item.second << '\n';
	}
}*/

/*static void countMxs(const BxtoMxs& bxtomxs, MxtoCount& counts) {
        counts.clear();
		assert(counts.empty());
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
}*/

static void filterMxs(unsigned thread_id, BxtoMxs& bxtomxs, unsigned num_threads, MxtoCount& counts, unsigned c, unsigned C, Mxs& rareMxs, Mxs& freqMxs) {
	assert (!bxtomxs.empty() && !counts.empty());
	for (unsigned index = thread_id; index < bxtomxs.size(); index+=num_threads) {
		auto it = std::next(bxtomxs.begin(), index);
		const auto& bx = it->first;
		auto& mxs = it->second;
		Mxs validMxs;
		validMxs.reserve(mxs.size());
		/*std::ostringstream oss;
		oss << std::this_thread::get_id() << " : " << bx << '\n';
		std::cout << oss.str();*/
		for (const auto& mx : mxs) {
			unsigned count = counts[mx];
			if (count < 2) {
				{
					std::lock_guard<std::mutex> lock(m2);
					//std::cerr << std::this_thread::get_id() << ' ';
					//std::cerr << bx << " : " << mx << " : " << "RARE\n";
					rareMxs.insert(mx);
				}
			} else if (count >= C) {
				{
					std::lock_guard<std::mutex> lock(m3); //because we are inserting to a different ds, lock a different mutex to save time
					//std::cerr << std::this_thread::get_id() << ' ';
					//std::cerr << bx << " : " << mx << " : " << "FREQ\n";
					freqMxs.insert(mx);
				}
			} else {
				validMxs.insert(mx);
			}
		}
		bxtomxs[bx] = std::move(validMxs);
	}
}

static void filterBarcodes(std::promise<int> toofew, std::promise<int> toomany, unsigned thread_id, BxtoMxs& bxtomxs, unsigned num_threads, unsigned n, unsigned N) {
	assert(!bxtomxs.empty());
	unsigned too_few = 0;
	unsigned too_many = 0;
	for (unsigned index = thread_id; index < bxtomxs.size(); index+=num_threads) {
		auto it = std::next(bxtomxs.begin(), index);
		const auto& bx = it->first;
		auto& mxs = it->second;
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

static std::vector<float> quantile (const MxtoCount& counts, unsigned c) {
	std::vector<int> values;
	values.reserve(counts.size());
	for(const auto& item : counts) {
		if (item.second < c) {
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

int main(int argc, char *argv[]){
	//BxtoMxs bxtomxs =  {{"BX1", {1,2,3,4}}, {"BX2",{4,5,6,9}}, {"BX3",{0,1,6,7,8}}, {"BX4", {4,5,6,7,8, 10}}, {"BX5", {1,2,3,4,5}}, {"BX6", {1,2,3,4}}, {"BX7", {0,6,7,8}}, {"BX8", {1,2,3,14}}, {"BX9", {4,5,6,9}}, {"BX10", {0, 12, 14}}, {"BX11", {0,4,11,14,8,7,9}}};
	t0 = std::chrono::steady_clock::now();
	auto progname = "physlr-filter-barcodes-minimizers";
	int x;
	int optindex = 0;
	static int help = 0;
	unsigned n = 0;
	unsigned N = 0;
	unsigned c = 0;
	unsigned C = 0;
	unsigned t = 1;
	bool silent = false;
	bool failed = false;
	bool n_set = false;
	bool N_set = false;
	char* end = nullptr;
	std::string outfile("/dev/stdout");
	static const struct option longopts[] = { { "help", no_argument, &help, 1 },{ nullptr, 0, nullptr, 0 } };
	while ((x = getopt_long(argc, argv, "o:s:n:N:c:C:t:", longopts, &optindex)) != -1) {
		switch (x) {
			case 0:
				break;
			case 'o':
				outfile.assign(optarg);
				break;
			case 's':
				silent = true;
				break;
			case 'n':
				n_set = true;
				n = strtoul(optarg, &end, 10);
				break;
			case 'N':
				N_set = true;
				N = strtoul(optarg, &end, 10);
				break;
			case 'c' :
				c = strtoul(optarg, &end, 10);
				break;
			case 'C':
				C = strtoul(optarg, &end, 10);
				break;
			case 't':
				t = strtoul(optarg, &end, 10);
				break;
			default:
				exit(EXIT_FAILURE);
			}
	}
	std::vector<std::string> infiles(&argv[optind], &argv[argc]);
	if (argc < 2) {
		printUsage(progname);
		exit(EXIT_FAILURE);
	}
	if (help != 0) {
		printUsage(progname);
		exit(EXIT_SUCCESS);
	}
	if (!n_set) {
		n = 1;
	}
	if (!N_set) {
		N = INT_MAX;
	}
	if (n == 0) {
		printErrorMsg(progname, "option has incorrect argument -- 'n'");
		failed = true;
	}
	if (N == 0) {
		printErrorMsg(progname, "option has incorrect argument -- 'N'");
		failed = true;
	}
	if (infiles.empty()) {
		printErrorMsg(progname, "missing file operand");
		failed = true;
	}
	if (failed) {
		exit(EXIT_FAILURE);
	}
	std::ofstream ofs(outfile);
	assert_good(ofs, outfile);
	for (auto &infile : infiles) {
		if (infile == "-") {
			infile = "/dev/stdin";
		}
		std::ifstream ifs(infile);
		assert_good(ifs, infile);
		BxtoMxs bxtomxs;
		MxtoCount counts;
		unsigned num_threads = t;
		std::vector <std::thread> t(num_threads);
		readMxs(ifs, infile, silent, bxtomxs);
		//countMxs(bxtomxs, counts);
		for (unsigned i =0; i< num_threads; ++i) {
			t[i] = std::thread(&countMxs, i, std::ref(bxtomxs), num_threads, std::ref(counts));
		}
		for (auto& thr : t) {
			if (thr.joinable()) {
				thr.join();
			}
		}
		//printCounts(counts);
		if(!silent) {
			auto t = std::chrono::steady_clock::now();
			auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(t - t0);
			std::cerr << "Time after counting minimizers (ms): " << diff.count() << '\n';
			std::cerr << "Counted " << counts.size() << " minimizers." << "\n\n";	
		}	
		if (c == 0) {
			c = 2;
		} 
		if (C == 0) {
			std::vector<float> q = quantile(counts, c);
			C = int(q[2] + 1.5 * (q[2] - q[0])); 
			std::cerr << "C is calculated to be " << C << '\n';
		}
		Mxs rareMxs;
		Mxs freqMxs;
		for (unsigned i =0; i< num_threads; ++i) {
			t[i] = std::thread(&filterMxs, i, std::ref(bxtomxs), num_threads, std::ref(counts), c, C, std::ref(rareMxs), std::ref(freqMxs));
		}
		for (auto& thr : t){
			if(thr.joinable()){
				thr.join();
			}
		}
		if (!silent) {
			auto t = std::chrono::steady_clock::now();
			auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(t - t0);
			std::cerr << "Time after filtering minimizers (ms): " << diff.count() << '\n';
			std::cerr << "Removed " << rareMxs.size() << " rare minimizers of " << counts.size() << " (" << std::setprecision(1) << std::fixed << 100.0 * rareMxs.size() / counts.size() << "%)\n";
			std::cerr << "Removed " << freqMxs.size() << " frequent minimizers of " << counts.size() << " (" <<     std::setprecision(1) << std::fixed << 100.0 * freqMxs.size() / counts.size() << "%)\n\n";
		}
		unsigned toofew = 0;
		unsigned toomany = 0;
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
		if (!silent) {
			auto t = std::chrono::steady_clock::now();
			auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(t - t0);
			std::cerr << "Time after filtering barcodes (ms): " << diff.count() << '\n';
			std::cerr << "Discarded " << toofew << " barcodes with too few minimizers of " << bxtomxs.size() << " (" << std::setprecision(1) << std::fixed << 100.0 * toofew / bxtomxs.size() << "%)\n";
			std::cerr << "Discarded " << toomany << " barcodes with too many minimizers of " << bxtomxs.size() << " (" << std::setprecision(1) << std::fixed << 100.0 * toomany / bxtomxs.size() << "%)\n\n";
		}
		writeMxs(bxtomxs, ofs, outfile, silent);
		/*for(const auto& item : bxtomxs) {
			std::cout << item.first << ' ';
			for(const auto& mx : item.second) {
				std::cout << mx << ' ';
			}
			std::cout << '\n';
		}*/
	}
	ofs.flush();
	assert_good(ofs, outfile);
	return 0;
}
