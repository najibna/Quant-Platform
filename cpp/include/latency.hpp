#pragma once

#include <chrono>
#include <string>
#include <vector>

// one timed sample
struct LatencySample {
    std::string stage;
    long long nanos;
};

// tiny timer for profiling stages
class Timer {
public:
    void start();
    long long stop_nanos();

private:
    std::chrono::high_resolution_clock::time_point t0_;
    bool running_ = false;
};

// collect samples and dump a quick summary
class LatencyProfiler {
public:
    void add(const std::string& stage, long long nanos);
    void print_summary() const;
    const std::vector<LatencySample>& samples() const;

private:
    std::vector<LatencySample> samples_;
};
