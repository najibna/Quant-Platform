#include "worker.hpp"

#include "latency.hpp"

#include <vector>

Stats run_research_job(const std::vector<Tick>& ticks, const ResearchJob& job, double& elapsed_ms) {
    Timer timer;
    timer.start();

    auto slice = slice_ticks(ticks, job.start, job.end);
    std::vector<double> prices;
    prices.reserve(slice.size());
    for (const auto& t : slice) {
        if (job.symbol.empty() || t.symbol == job.symbol) {
            prices.push_back(t.price);
        }
    }

    Stats s = compute_stats(prices);
    elapsed_ms = static_cast<double>(timer.stop_nanos()) / 1e6;
    return s;
}
