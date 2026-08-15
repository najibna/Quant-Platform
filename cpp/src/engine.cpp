#include "engine.hpp"

#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

namespace {
std::mutex feed_mu;
std::mutex signal_mu;
}

TradingEngine::TradingEngine(std::vector<Tick> ticks) : ticks_(std::move(ticks)) {}

const std::vector<Order>& TradingEngine::orders() const {
    return orders_;
}

const LatencyProfiler& TradingEngine::profiler() const {
    return profiler_;
}

int TradingEngine::evaluate_signal(double price) {
    // keep a short rolling mean in a static so the signal has memory
    static double ema = 0.0;
    static bool init = false;
    const double alpha = 0.2;

    if (!init) {
        ema = price;
        init = true;
        return 0;
    }

    ema = alpha * price + (1.0 - alpha) * ema;
    double diff = price - ema;

    // buy if price dips under ema, sell if it runs above
    if (diff < -0.05) return 1;
    if (diff > 0.05) return -1;
    return 0;
}

void TradingEngine::feed_loop() {
    for (const auto& t : ticks_) {
        Timer timer;
        timer.start();
        {
            std::lock_guard<std::mutex> lock(feed_mu);
            feed_q_.push_back(t);
        }
        {
            std::lock_guard<std::mutex> lock(profile_mu_);
            profiler_.add("feed", timer.stop_nanos());
        }
        // tiny sleep so threads actually overlap a bit
        std::this_thread::sleep_for(std::chrono::microseconds(50));
    }
    feed_done_ = true;
}

void TradingEngine::signal_loop() {
    size_t seen = 0;
    while (true) {
        Tick tick;
        bool have = false;
        {
            std::lock_guard<std::mutex> lock(feed_mu);
            if (seen < feed_q_.size()) {
                tick = feed_q_[seen++];
                have = true;
            }
        }

        if (!have) {
            if (feed_done_) break;
            std::this_thread::sleep_for(std::chrono::microseconds(20));
            continue;
        }

        Timer timer;
        timer.start();
        int sig = evaluate_signal(tick.price);
        if (sig != 0) {
            Order o;
            o.symbol = tick.symbol;
            o.side = (sig > 0) ? "buy" : "sell";
            o.price = tick.price;
            o.size = 10;
            o.ts = tick.ts;
            o.status = "pending";
            std::lock_guard<std::mutex> lock(signal_mu);
            signal_q_.push_back(o);
        }
        {
            std::lock_guard<std::mutex> lock(profile_mu_);
            profiler_.add("signal", timer.stop_nanos());
        }
    }
    signal_done_ = true;
}

void TradingEngine::order_loop() {
    size_t seen = 0;
    while (true) {
        Order o;
        bool have = false;
        {
            std::lock_guard<std::mutex> lock(signal_mu);
            if (seen < signal_q_.size()) {
                o = signal_q_[seen++];
                have = true;
            }
        }

        if (!have) {
            if (signal_done_) break;
            std::this_thread::sleep_for(std::chrono::microseconds(20));
            continue;
        }

        Timer timer;
        timer.start();
        // pretend we hit the exchange
        o.status = "filled";
        orders_.push_back(o);
        {
            std::lock_guard<std::mutex> lock(profile_mu_);
            profiler_.add("order", timer.stop_nanos());
        }
    }
}

void TradingEngine::run() {
    std::thread feed_th(&TradingEngine::feed_loop, this);
    std::thread signal_th(&TradingEngine::signal_loop, this);
    std::thread order_th(&TradingEngine::order_loop, this);

    feed_th.join();
    signal_th.join();
    order_th.join();
}
