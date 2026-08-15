#include "engine.hpp"
#include "market_data.hpp"
#include "stats.hpp"

#include <fstream>
#include <iostream>
#include <vector>

// write filled orders so python can pick them up
static void write_orders(const std::string& path, const std::vector<Order>& orders) {
    std::ofstream out(path);
    out << "symbol,side,price,size,ts,status\n";
    for (const auto& o : orders) {
        out << o.symbol << "," << o.side << "," << o.price << ","
            << o.size << "," << o.ts << "," << o.status << "\n";
    }
}

// write latency samples for the report script
static void write_latency(const std::string& path, const LatencyProfiler& profiler) {
    std::ofstream out(path);
    out << "stage,nanos\n";
    for (const auto& s : profiler.samples()) {
        out << s.stage << "," << s.nanos << "\n";
    }
}

int main(int argc, char** argv) {
    std::string path = "data/sample_ticks.csv";
    if (argc > 1) {
        path = argv[1];
    }

    std::cout << "loading ticks from " << path << "\n";
    auto ticks = load_ticks(path);
    if (ticks.empty()) {
        std::cerr << "no ticks loaded\n";
        return 1;
    }
    std::cout << "loaded " << ticks.size() << " ticks\n";

    // quick price stats before we trade
    std::vector<double> prices;
    for (const auto& t : ticks) {
        prices.push_back(t.price);
    }
    print_stats("all ticks", compute_stats(prices));

    TradingEngine engine(ticks);
    std::cout << "starting engine (feed / signal / order threads)\n";
    engine.run();

    std::cout << "filled " << engine.orders().size() << " orders\n";
    engine.profiler().print_summary();

    write_orders("data/processed/orders.csv", engine.orders());
    write_latency("data/processed/latency.csv", engine.profiler());
    std::cout << "wrote data/processed/orders.csv and latency.csv\n";
    return 0;
}
