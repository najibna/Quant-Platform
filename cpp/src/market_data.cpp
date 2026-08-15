#include "market_data.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

// split a csv line into fields
static std::vector<std::string> split_csv(const std::string& line) {
    std::vector<std::string> out;
    std::stringstream ss(line);
    std::string part;
    while (std::getline(ss, part, ',')) {
        out.push_back(part);
    }
    return out;
}

std::vector<Tick> load_ticks(const std::string& path) {
    std::vector<Tick> ticks;
    std::ifstream in(path);
    if (!in) {
        std::cerr << "could not open " << path << "\n";
        return ticks;
    }

    std::string line;
    // skip header
    std::getline(in, line);

    while (std::getline(in, line)) {
        if (line.empty()) {
            continue;
        }
        auto parts = split_csv(line);
        if (parts.size() < 4) {
            continue;
        }
        Tick t;
        t.symbol = parts[0];
        t.ts = std::stod(parts[1]);
        t.price = std::stod(parts[2]);
        t.size = std::stoi(parts[3]);
        ticks.push_back(t);
    }
    return ticks;
}

std::vector<Tick> filter_symbol(const std::vector<Tick>& ticks, const std::string& symbol) {
    std::vector<Tick> out;
    for (const auto& t : ticks) {
        if (t.symbol == symbol) {
            out.push_back(t);
        }
    }
    return out;
}

std::vector<Tick> slice_ticks(const std::vector<Tick>& ticks, size_t start, size_t end) {
    if (start >= ticks.size()) {
        return {};
    }
    if (end > ticks.size()) {
        end = ticks.size();
    }
    return std::vector<Tick>(ticks.begin() + static_cast<long>(start),
                             ticks.begin() + static_cast<long>(end));
}
