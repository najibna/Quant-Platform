#pragma once

#include "market_data.hpp"
#include "stats.hpp"

#include <string>

// one research job for a worker process
struct ResearchJob {
    int worker_id;
    std::string symbol;
    size_t start;
    size_t end;
};

// process a slice of ticks and return stats
Stats run_research_job(const std::vector<Tick>& ticks, const ResearchJob& job, double& elapsed_ms);
