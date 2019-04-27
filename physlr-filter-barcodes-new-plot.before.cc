#include <algorithm>
#include <cassert>
#include <chrono>
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
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set> 
#include <vector>
#include "tsl/robin_map.h"
#include "tsl/robin_set.h"
#include <boost/range/algorithm/count.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/assign.hpp>

using Mx = uint64_t;
using Mxs = std::vector<Mx>;
using Bx = std::string;
using BxtoMxs = tsl::robin_map<Bx, Mxs>;
using MxtoCount = std::unordered_map<Mx, unsigned>;

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
	std::cout << "Usage:  " << progname
		<< "  -n n -N N -c c -C C [-s] [-b] [-m] [-o file] file...\n\n"
		"  -s         silent; disable verbose output\n"
		"  -o file    write output to file, default is stdout\n"
		"  -n         minimum number of minimizers per barcode\n"
		"  -N         maximum number of minimizers per barcode\n"
		"  -c         keep minimizers with equal to or greater than c multiplicity\n"
		"  -C         keep minimizers with less than C multiplicity\n"
		"  -b         store minimizer count per barcode to file\n"
        "  -m         store multiplicity per minimizer to file\n"
		"  --help     display this help and exit\n"
		"  file       space separated list of FASTQ files\n";
}

static void read_minimizers(std::istream &is, const std::string &ipath, bool silent, MxtoCount& counts, BxtoMxs& bxtomxs) {
	if (is.peek() == std::ifstream::traits_type::eof()) {
		std::cerr << "physlr-filter-barcodes-minimizers: error: Empty input file: " << ipath << '\n';
		exit(EXIT_FAILURE);
	}
	assert(bxtomxs.empty() && counts.empty()); 
	Bx bx;
	std::string mx;
	std::string mx_line;
	while ((is >> bx) && (getline(is, mx_line))) {
		bxtomxs[bx].reserve(boost::count(mx_line,' ')+1);
		std::istringstream iss(mx_line);
		while (iss >> mx) {
			Mx mx_num = strtoull(mx.c_str(), nullptr, 0);
			bxtomxs[bx].push_back(mx_num);
			if (counts.find(mx_num) == counts.end()) {
				counts[mx_num] = 1;
			} else {
				++counts[mx_num];
			}
		}
	}
	auto t = std::chrono::steady_clock::now();
	auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(t - t0);
	if (!silent) {
		std::cerr << "Time after reading (ms): " << diff.count() << '\n';
	}
}

static std::vector<float> quantile(std::vector<float> quantiles, std::vector<unsigned> values){
	std::sort(values.begin(), values.end());
	std::vector<float> qs;
	for (const auto& p : quantiles){
			qs.push_back(values[round(p*(values.size()-1))]);
	}
	return qs;
}

static void filter_minimizers(unsigned c, unsigned C, bool silent, BxtoMxs& bxtomxs, MxtoCount& counts){
	unsigned count = counts.size();
	//remove rare minimizers
	if(c == 0){
	    c = 2;
	}
	unsigned rare = 0;
	for(auto it = bxtomxs.begin(); it != bxtomxs.end(); ++it){
	    auto& bx = it->first;
	    auto mxs = it->second;
		Mxs not_rare;
		not_rare.reserve(mxs.size());
		for(const auto& mx : mxs){
			if(counts[mx] >= c){
				not_rare.push_back(mx);	
			}
			else{
				++rare;
			}
		}
		bxtomxs[bx] = std::move(not_rare);
	}
	//update counts
	for(auto it = counts.begin(); it != counts.end(); ){
		auto& mx = it->first;
		if(counts[mx] < c){
			it = counts.erase(it);
		}
		else{
			++it;
		}
	}
	//compute quantiles and remove repetitive minimizers
	std::vector<unsigned> values;
	values.reserve(counts.size());
	boost::copy(counts | boost::adaptors::map_values, std::back_inserter(values));
	std::vector<float> q = {0.25,0.5,0.75};
	q = quantile(q, values);
	unsigned high_whisker = int(int(q[2]) + 1.5 * (int(q[2]) - int(q[0])));	
	if(C == 0){
		C = high_whisker;
	}
	std::cerr << "C = " << C << '\n';
	unsigned repetitive = 0;
	for(auto it = bxtomxs.begin(); it != bxtomxs.end(); ++it){
		auto& bx = it->first;
		auto mxs = it->second;
		Mxs not_repetitive;
		not_repetitive.reserve(mxs.size());
		for(const auto& mx : mxs){
			if(counts[mx] < C){
				not_repetitive.push_back(mx);
			}
			else{
				++repetitive;
			}
		}
		bxtomxs[bx] = std::move(not_repetitive);
	}
	auto t = std::chrono::steady_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(t - t0);
	if(!silent){
		std::cerr << "Time after filtering minimizers (ms): " << diff.count() << '\n';
		std::cerr << "Removed " << rare << " rare minimizers of " << count << " (" << std::setprecision(1) << std::fixed << 100.0 * rare / count << "%)\n";
		std::cerr << "Removed " << repetitive << " repetitive minimizers of " << count << " (" << std::setprecision(1) << std::fixed << 100.0 * repetitive / count << "%)\n";
	}
	//update counts once more (WE DON'T HAVE TO DO THIS IF WE WON'T AGAIN CHECK FOR COUNTS WHEN FILTERING BARCODES -- WILL WE ?? )
	/*for(auto it = counts.begin(); it != counts.end(); ){
	    auto& mx = it->first;
	    if(counts[mx] >= C){
		    it = counts.erase(it);
		}
		else{
	     	++it;
	    }
	}*/
}

static void filter_barcodes_write(std::ostream& os, const std::string& opath, unsigned n, unsigned N, bool silent, BxtoMxs& bxtomxs){
	unsigned too_few = 0;
	unsigned too_many = 0;
	unsigned initial_size = bxtomxs.size();
	for(auto it = bxtomxs.begin(); it != bxtomxs.end(); ){
		auto& bx = it->first;
		auto& mxs = it->second;
		if(mxs.size() < n){
			++too_few;
			it = bxtomxs.erase(it);
		}
		else if(mxs.size() >= N){
			++too_many;
			it = bxtomxs.erase(it);
		}
		else{
			std::ostringstream oss;
			oss << bx;
			char sep = '\t';
			for(const auto& mx : mxs){
				oss << sep << mx;
				sep = ' ';
			}
			oss << '\n';
			os << oss.str();
			assert_good(oss,opath);
			++it;
		}
	}
	assert_good(os,opath);
	auto t = std::chrono::steady_clock::now();
	auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(t - t0);
	if (!silent) {
			std::cerr << "Time after filtering barcodes and writing (ms): " << diff.count() << '\n';	
			std::cerr << "Discarded " << too_few << " barcodes with too few minimizers of " << initial_size << " (" << std::setprecision(1) << std::fixed << 100.0 * too_few / initial_size << "%)\n";
			std::cerr << "Discarded " << too_many << " barcodes with too many minimizers of " << initial_size << " (" << std::setprecision(1) << std::fixed << 100.0 * too_many / initial_size << "%)\n";
			std::cerr << "Wrote " << initial_size - too_few - too_many << " barcodes\n";
	}
}

static void write_bx_depth(BxtoMxs bxtomxs, std::ostream& os, const std::string& opath){
	os << "Barcode-Depth\n";
	for(const auto& item : bxtomxs){
		os << item.second.size() << '\n';
	}
	assert_good(os,opath);
}
static void write_mx_depth(MxtoCount counts, std::ostream& os, const std::string& opath){
	os << "Minimizer-Depth\n";
	for(const auto& item : counts){
		os << item.second << '\n';
	}
	assert_good(os,opath);
}

int main(int argc, char *argv[])
{
	t0 = std::chrono::steady_clock::now();
	auto progname = "physlr-filter-barcodes-minimizers";
	int param;
	int optindex = 0;
	static int help = 0;
	unsigned n = 0;
	unsigned N = 0;
	unsigned c = 0;
	unsigned C = 0;
	bool silent = false;
	bool failed = false;
	bool bd_set = false;
    bool md_set = false;
	bool n_set = false;
	bool N_set = false;
	char* end = nullptr;
	std::string outfile("/dev/stdout");
	std::string bdfile("/dev/stdout");
    std::string mdfile("/dev/stdout");
	static const struct option longopts[] = { { "help", no_argument, &help, 1 },{ nullptr, 0, nullptr, 0 } };
	while ((param = getopt_long(argc, argv, "o:s:n:N:c:C:b:m:", longopts, &optindex)) != -1) {
		switch (param) {
		case 0:
			break;
		case 'o':
			outfile.assign(optarg);
			break;
		case 'b':
			bd_set = true;
			bdfile.assign(optarg);
			break;
		case 'm':
			md_set = true;
			mdfile.assign(optarg);
			break;
		case 's':
			silent = true;
			break;
		case 'c':
			c = strtoul(optarg, &end, 10);
			break;
		case 'C':
			C = strtoul(optarg, &end, 10);
            break;
		case 'n':
			n_set = true;
			n = strtoul(optarg, &end, 10);
			break;
		case 'N':
			N_set = true;
			N = strtoul(optarg, &end, 10);
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
	std::ofstream ofsb(bdfile);
    assert_good(ofsb, bdfile);
    std::ofstream ofsm(mdfile);
    assert_good(ofsm, mdfile);
	for (auto &infile : infiles) {
		if (infile == "-") {
			infile = "/dev/stdin";
		}
		std::ifstream ifs(infile);
		assert_good(ifs, infile);
		MxtoCount counts; 
		BxtoMxs bxtomxs;
		read_minimizers(ifs, infile, silent, counts, bxtomxs);
		if(md_set){
			write_mx_depth(counts,ofsm,mdfile);
		}
		filter_minimizers(c, C, silent, bxtomxs, counts);
		if(bd_set){
			write_bx_depth(bxtomxs,ofsb,bdfile);
		}
		filter_barcodes_write(ofs, outfile, n, N, silent, bxtomxs);
	}
	ofs.flush();
	assert_good(ofs, outfile);
	return 0;
}
