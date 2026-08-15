#pragma once

#include "latency.hpp"
#include "market_data.hpp"

#include <mutex>
#include <string>
#include <vector>

// tiny order record
struct Order {
    std::string symbol;
    std::string side; // buy or sell
    double price;
    int size;
    double ts;
    std::string status;
};

// multithreaded trading engine
// feed thread pushes ticks, signal thread decides, order thread executes
class TradingEngine {
public:
    explicit TradingEngine(std::vector<Tick> ticks);

    // run all threads and wait until done
    void run();

    const std::vector<Order>& orders() const;
    const LatencyProfiler& profiler() const;

private:
    void feed_loop();
    void signal_loop();
    void order_loop();

    // very small mean reversion style signal
    int evaluate_signal(double price);

    std::vector<Tick> ticks_;
    std::vector<Tick> feed_q_;
    std::vector<Order> signal_q_;
    std::vector<Order> orders_;

    LatencyProfiler profiler_;
    std::mutex profile_mu_;
    bool feed_done_ = false;
    bool signal_done_ = false;
};
