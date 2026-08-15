#include "market_data.hpp"
#include "stats.hpp"
#include "worker.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

// worker process:
// takes an id, symbol filter, and a [start,end) tick range
// writes one stats line to an output file

int main(int argc, char** argv) {
    if (argc < 7) {
        std::cerr << "usage: worker <id> <csv> <symbol> <start> <end> <out>\n";
        return 1;
    }

    int worker_id = std::atoi(argv[1]);
    std::string csv = argv[2];
    std::string symbol = argv[3];
    size_t start = static_cast<size_t>(std::atoi(argv[4]));
    size_t end = static_cast<size_t>(std::atoi(argv[5]));
    std::string out_path = argv[6];

    auto ticks = load_ticks(csv);
    if (ticks.empty()) {
        std::cerr << "worker " << worker_id << " got no ticks\n";
        return 1;
    }

    ResearchJob job{worker_id, symbol, start, end};
    double elapsed_ms = 0.0;
    Stats s = run_research_job(ticks, job, elapsed_ms);

    std::ofstream out(out_path, std::ios::app);
    // worker_id,symbol,mean,std,count,elapsed_ms
    out << worker_id << "," << symbol << "," << s.mean << ","
        << s.stddev << "," << s.count << "," << elapsed_ms << "\n";

    std::cout << "worker " << worker_id
              << " symbol=" << symbol
              << " ticks=" << s.count
              << " mean=" << s.mean
              << " ms=" << elapsed_ms
              << "\n";
    return 0;
}
