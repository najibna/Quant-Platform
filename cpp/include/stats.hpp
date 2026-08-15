#pragma once

#include <string>
#include <vector>

// basic summary numbers for a price series
struct Stats {
    double mean;
    double stddev;
    double min_v;
    double max_v;
    size_t count;
};

// pull prices out of ticks and compute stats
Stats compute_stats(const std::vector<double>& values);

// simple returns between consecutive prices
std::vector<double> simple_returns(const std::vector<double>& prices);

// print stats in a short line
void print_stats(const std::string& label, const Stats& s);
