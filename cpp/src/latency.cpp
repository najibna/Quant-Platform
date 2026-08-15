#include "latency.hpp"

#include <iostream>
#include <map>

void Timer::start() {
    t0_ = std::chrono::high_resolution_clock::now();
    running_ = true;
}

long long Timer::stop_nanos() {
    if (!running_) {
        return 0;
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    running_ = false;
    return std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0_).count();
}

void LatencyProfiler::add(const std::string& stage, long long nanos) {
    samples_.push_back({stage, nanos});
}

const std::vector<LatencySample>& LatencyProfiler::samples() const {
    return samples_;
}

void LatencyProfiler::print_summary() const {
    // group by stage and print average
    std::map<std::string, std::pair<long long, int>> agg;
    for (const auto& s : samples_) {
        auto& a = agg[s.stage];
        a.first += s.nanos;
        a.second += 1;
    }

    std::cout << "latency summary (ns):\n";
    for (const auto& kv : agg) {
        double avg = static_cast<double>(kv.second.first) / kv.second.second;
        std::cout << "  " << kv.first
                  << " n=" << kv.second.second
                  << " avg=" << avg
                  << "\n";
    }
}
