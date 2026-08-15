#pragma once

#include <string>
#include <vector>

// one market tick
struct Tick {
    std::string symbol;
    double ts;
    double price;
    int size;
};

// load ticks from a csv file
std::vector<Tick> load_ticks(const std::string& path);

// keep only one symbol
std::vector<Tick> filter_symbol(const std::vector<Tick>& ticks, const std::string& symbol);

// grab a slice [start, end)
std::vector<Tick> slice_ticks(const std::vector<Tick>& ticks, size_t start, size_t end);
