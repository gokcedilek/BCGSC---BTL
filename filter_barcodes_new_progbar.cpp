#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>
#include <iomanip>
#include <getopt.h>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iterator>
#include <map>
#include "cpptqdm/tqdm.h" //remember to include the package!! the modified version is in: /projects/amp/honeybee/gdilek/physlr_task/project2

static inline void assert_good(const std::ios& stream, const std::string& path)
{
	if (!stream.good()) {
		std::cerr << "error: " << strerror(errno) << ": " << path << '\n';
		//std::cout << "Exiting from assert_good!";
		exit(EXIT_FAILURE);
	}
}

static void printErrorMsg(const std::string &progname, const std::string &msg)
{
	std::cerr << progname << ": " << msg << "\nTry 'physlr-indexlr --help' for more information.\n";
}

static void printUsage(const std::string &progname)
{
	std::cout << "Usage:  " << progname
		<< "  -k K -w W [-v] [-o file] file...\n\n"
		"  -k K       use K as k-mer size\n"
		"  -w W       use W as sliding-window size\n"
		"  -s         silent; disable verbose output\n"
		"  -o file    write output to file, default is stdout\n"
		"  -n         minimum number of minimizers per barcode\n"
		"  -N         maximum number of minimizers per barcode\n"
		"  --help     display this help and exit\n"
		"  file       space separated list of FASTQ files\n";
}

static std::unordered_set<uint64_t> splitMarkers(const std::string marker_line) {
	std::unordered_set<uint64_t> marker_set;
	std::istringstream iss(marker_line);
	std::string marker;
	while (iss >> marker != nullptr) {
		marker_set.insert(strtoull(marker.c_str(), nullptr, 0)); //marker: string --> unsigned long long integer!!
	}
	return marker_set;
}

static std::unordered_map<std::string, std::unordered_set<uint64_t>> read_minimizers(std::istream &is, const std::string &ipath, bool silent) {
	if (is.peek() == std::ifstream::traits_type::eof()) {
		std::cerr << "physlr-indexlr: error: Empty input file: " << ipath << '\n';
		exit(EXIT_FAILURE);
	}
	std::unordered_map<std::string, std::unordered_set<uint64_t>> bxtomin;
	std::string bx;
	std::string marker_line;
	while (((is >> bx) != nullptr) && (getline(is, marker_line) != nullptr)) {
		std::unordered_set<uint64_t> marker_set;
		for (uint64_t i : splitMarkers(marker_line)) {
			marker_set.insert(i);
		}
		if (bx.empty() || marker_set.empty()) {
			continue;
		}
		bxtomin[bx] = marker_set;
	}
	if(!silent){
	    std::cerr << "Reading " << ipath << "...\n";
        int N=2000;
        tqdm bar;
        bar.disable_colors();
        for(int i = 0; i < N; i++) {
            bar.progress(i, N);
            usleep(400);
        }
        bar.finish();
        std::cerr << "Read " << ipath << ".\n";
    }

	return bxtomin;
}

static void write(std::unordered_map<std::string, std::unordered_set<uint64_t>> bxtomin, std::ostream &os, const std::string &opath) {
	/*for (auto it = bxtomin.begin(); it != bxtomin.end(); ++it) {
	os << it->first << '\t';
	std::copy(it->second.begin(), it->second.end(), std::ostream_iterator<uint64_t>(os, " "));
	os << '\n';*/
	std::string bx;
	std::unordered_set<uint64_t> markers;
	for (std::pair< std::string, std::unordered_set<uint64_t> > item : bxtomin) {
		bx = item.first;
		markers = item.second;
		os << bx;
		char sep = '\t';
		for (auto m : markers) {
			os << sep << m;
			sep = ' ';
		}
		os << '\n';
		assert_good(os, opath);
	}
}

static std::unordered_map<uint64_t, unsigned int> counterForRemove(std::unordered_map<std::string, std::unordered_set<uint64_t>> bxtomin) {
	std::unordered_map<uint64_t, unsigned int> counts;
	std::unordered_set<uint64_t> markers;
	for (std::pair< std::string, std::unordered_set<uint64_t> > item : bxtomin)
	{
		markers = item.second;
		for (uint64_t marker : markers) {
			if (counts.find(marker) == counts.end()) {
				counts[marker] = 1;
			}
			else {
				counts[marker] = counts.at(marker) + 1;
			}
		}
	}
	return counts;

}

static std::unordered_map<std::string, std::unordered_set<uint64_t>> remove_singleton_markers(std::unordered_map<std::string, std::unordered_set<uint64_t>> bxtomin, bool silent) {
	std::unordered_map<uint64_t, unsigned int> counts = counterForRemove(bxtomin); //map from marker to count
	std::unordered_set<uint64_t> singletons;
	for (std::pair<uint64_t, unsigned int> item : counts) {
		if (item.second < 2) {
			singletons.insert(item.first); //markers that occur once
		}
	}

	std::unordered_set<uint64_t> markers;
	std::unordered_set<uint64_t> allUniqueMarkers;
	std::unordered_map<std::string, std::unordered_set<uint64_t>> new_map;
	for (std::pair < std::string, std::unordered_set<uint64_t> > item : bxtomin) {
		markers = item.second;
		std::unordered_set<uint64_t> new_markers;
		for (uint64_t marker : markers) {
			if (!allUniqueMarkers.count(marker)) {
               //means this marker doesn't exist
               allUniqueMarkers.insert(marker);
            }
			if (singletons.find(marker) == singletons.end()) {
				new_markers.insert(marker);
			}
		}
		new_map[item.first] = new_markers;
	}
	if(!silent){
	    std::cerr << "Counted " << allUniqueMarkers.size() << " minimizers.\n";
        int N=2000;
        tqdm bar;
        bar.disable_colors();
        for(int i = 0; i < N; i++) {
            bar.progress(i, N);
            usleep(400);
        }
        bar.finish();
	    std::cerr << "Removed " << singletons.size() << " minimizers that occur only once of " << allUniqueMarkers.size() << " (" << std::setprecision(1) << std::fixed << 100.0 * singletons.size() / allUniqueMarkers.size() << "%).\n";
	}
	return new_map;
}



static void physlr_filterbarcodes(std::istream &is, const std::string &ipath, std::ostream &os, const std::string &opath, const size_t n, const size_t N, bool silent) {
	if (is.peek() == std::ifstream::traits_type::eof()) {
		std::cerr << "physlr-filterbarcodes: error: Empty input file: " << ipath << '\n';
		exit(EXIT_FAILURE);
	}
	std::unordered_map<std::string, std::unordered_set<uint64_t>> bxtomin = read_minimizers(is, ipath, silent);
	int initial_size = bxtomin.size();
	bxtomin = remove_singleton_markers(bxtomin, silent);
	std::string bx;
	std::unordered_set<uint64_t> markers;
	unsigned int too_few = 0, too_many = 0;
	//for (std::pair< std::string, std::unordered_set<uint64_t> > item : bxtomin) {
	//	bx = item.first;
	//	markers = item.second;
	//	if (markers.size() < n) {
	//		too_few++;
	//	}
	//	else if (markers.size() >= N) {
	//		too_many++;
	//	}
	//	else {
	//		std::cout << bx << "\t";
	//		std::copy(markers.begin(), markers.end(), std::ostream_iterator<uint64_t>(std::cout, " ")); //minimizers
	//		std::cout << " " << '\n';
	//		std::cout << bx;
	//		char sep = '\t';
	//		for (auto m : markers) {
	//			std::cout << sep << m;
	//			sep = ' ';
	//		}
	//		std::cout << '\n';
	//	}
	//}
	for (auto it = bxtomin.begin(); it != bxtomin.end(); ) {
		bx = it->first;
		markers= it->second;
		if (markers.size() < n) {
			too_few++;
			it = bxtomin.erase(it);
		}
		else if (markers.size() >= N) {
			too_many++;
			it = bxtomin.erase(it);
		}
		else {
			//don't remove
			it++;
		}
	}
	if(!silent){
	std::cerr << "Discarded " << too_few << " barcodes with too few markers of " << initial_size << " (" << std::setprecision(1) << std::fixed << 100.0 * too_few / initial_size << "%).\n";
	std::cerr << "Discarded " << too_many << " barcodes with too many markers of " << initial_size << " (" << std::setprecision(1) << std::fixed << 100.0 * too_many / initial_size << "%).\n";
	int N=2000;
    tqdm bar;
    bar.disable_colors();
    for(int i = 0; i < N; i++) {
        bar.progress(i, N);
        usleep(400);
    }
    bar.finish();
	std::cerr << "Wrote " << initial_size - too_few - too_many << " barcodes.\n";
	}
	//clock_t t = clock() - t0;
	//std::cerr << float(t) / clocks_per_sec << "wrote" << bxtomin.size() - too_few - too_many << "barcodes";
	//auto stop = std::chrono::steady_clock::now();
	//std::cerr << std::chrono::duration_cast<std::chrono::milliseconds>(stop-start).count()/1000 << " wrote " << bxtomin.size() - too_few - too_many << " barcodes";
	write(bxtomin, os, opath);
}


int main(int argc, char *argv[])
{
	auto progname = "physlr-filterbarcodes";
	int      c;
	int      optindex = 0;
	static int help = 0;
	unsigned k = 0;
	unsigned w = 0;
	unsigned n = 0;
	double N = 0;
	bool     silent = false;
	bool     failed = false;
	bool     w_set = false;
	bool     k_set = false;
	bool     n_set = false;
	bool     N_set = false;
	char     *end = nullptr;
	std::string outfile("/dev/stdout");
	static const struct option longopts[] = { { "help", no_argument, &help, 1 },{ nullptr, 0, nullptr, 0 } };
	while ((c = getopt_long(argc, argv, "k:w:o:s:n:N:", longopts, &optindex)) != -1) {
		switch (c) {
		case 0:
			break;
		case 'k':
			k_set = true;
			k = strtoul(optarg, &end, 10);
			break;
		case 'w':
			w_set = true;
			w = strtoul(optarg, &end, 10);
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
		default:
			//std::cout << "Exiting from default!";
			exit(EXIT_FAILURE);
		}
	}
	std::vector<std::string> infiles(&argv[optind], &argv[argc]);
	if (argc < 2) {
		printUsage(progname);
		//std::cout << "Exiting from argc<2!";
		exit(EXIT_FAILURE);
	}
	if (help != 0) {
		printUsage(progname);
		//std::cout << "Exiting from help!";
		exit(EXIT_SUCCESS);
	}
	else if (!k_set) {
		printErrorMsg(progname, "missing option -- 'k'");
		failed = true;
	}
	else if (!w_set) {
		printErrorMsg(progname, "missing option -- 'w'");
		failed = true;
	}
	else if (!n_set) {
		n = 1;
		/*printErrorMsg(progname, "missing option -- 'n'");
		failed = true;*/
	}
	else if (!N_set) {
		N = std::numeric_limits<double>::infinity();
		/*printErrorMsg(progname, "missing option -- 'N'");
		failed = true;*/
	}
	else if (k == 0) {
		printErrorMsg(progname, "option has incorrect argument -- 'k'");
		failed = true;
	}
	else if (w == 0) {
		printErrorMsg(progname, "option has incorrect argument -- 'w'");
		failed = true;
	}
	else if (n == 0) {
		printErrorMsg(progname, "option has incorrect argument -- 'n'");
		failed = true;
	}
	else if (N == 0) {
		printErrorMsg(progname, "option has incorrect argument -- 'N'");
		failed = true;
	}
	else if (infiles.empty()) {
		printErrorMsg(progname, "missing file operand");
		failed = true;
	}
	if (failed) {
		//std::cout << "Exiting from failed!";
		exit(EXIT_FAILURE);
	}
	std::ofstream ofs(outfile);
	assert_good(ofs, outfile);
	for (auto &infile : infiles) {
		//std::cout << "Entered the loop!";
		if (infile == "-")
			infile = "/dev/stdin";
		std::ifstream ifs(infile);
		assert_good(ifs, infile);
		//std::cout << "In the loop!";
		//minimizeReads(ifs, infile, ofs, outfile, k, w, verbose);
		physlr_filterbarcodes(ifs, infile, ofs, outfile, n, N , silent);
	}
	ofs.flush();
	assert_good(ofs, outfile);
	//std::cout << "Reached the end!";

	return 0;
}
