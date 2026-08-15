#include "stats.hpp"

#include <cmath>
#include <iostream>

Stats compute_stats(const std::vector<double>& values) {
    Stats s{};
    s.count = values.size();
    if (values.empty()) {
        return s;
    }

    double sum = 0.0;
    s.min_v = values[0];
    s.max_v = values[0];

    for (double v : values) {
        sum += v;
        if (v < s.min_v) s.min_v = v;
        if (v > s.max_v) s.max_v = v;
    }
    s.mean = sum / static_cast<double>(values.size());

    double acc = 0.0;
    for (double v : values) {
        double d = v - s.mean;
        acc += d * d;
    }
    s.stddev = std::sqrt(acc / static_cast<double>(values.size()));
    return s;
}

std::vector<double> simple_returns(const std::vector<double>& prices) {
    std::vector<double> rets;
    if (prices.size() < 2) {
        return rets;
    }
    for (size_t i = 1; i < prices.size(); ++i) {
        if (prices[i - 1] == 0.0) {
            continue;
        }
        rets.push_back((prices[i] - prices[i - 1]) / prices[i - 1]);
    }
    return rets;
}

void print_stats(const std::string& label, const Stats& s) {
    std::cout << label
              << " count=" << s.count
              << " mean=" << s.mean
              << " std=" << s.stddev
              << " min=" << s.min_v
              << " max=" << s.max_v
              << "\n";
}
