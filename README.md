# Quant Platform

Distributed quantitative research and low-latency trading stack built with **C++**, **Python**, and **SQL**.

The platform ingests market data, runs a multithreaded trading engine, backtests strategies, computes statistics, and profiles end-to-end latency. Research workloads can be split across worker processes for parallel analysis.

## Features

- **Market data ingest** — load tick data from CSV into SQLite
- **Low-latency trading engine** — multithreaded C++ pipeline for feed handling, signal evaluation, and order execution
- **Strategy backtesting** — Python tooling for mean-reversion signals and PnL reporting
- **Statistics** — price summary metrics computed in C++ research workers
- **Latency profiling** — per-stage timing (feed / signal / order) with SQL persistence
- **Distributed research** — fan-out tick ranges across multiple worker processes

## Tech Stack

| Area | Tools |
|------|--------|
| Engine & workers | C++17, pthreads |
| Tooling | Python 3 |
| Storage | SQLite |
| Build | Make |
| OS target | Linux / macOS |

## Project Layout

```
cpp/        Trading engine and research workers
python/     Ingest, signals, backtest, and reporting
sql/        Database schema
data/       Sample market data
scripts/    Demo and distributed research runners
```

## Requirements

- `g++` with C++17 support
- Python 3
- SQLite 3

No third-party Python packages are required (stdlib only).

## Build

```bash
make
```

This produces:

- `bin/engine` — multithreaded trading engine
- `bin/worker` — research worker for distributed jobs

## Quick Start

Run the full demo:

```bash
./scripts/run_platform.sh
```

Or step through individually:

```bash
# ingest sample ticks into sqlite
python3 python/ingest.py

# run the trading engine
./bin/engine data/sample_ticks.csv

# persist orders and print latency summary
python3 python/store_results.py orders
python3 python/latency_report.py

# backtest
python3 python/backtest.py AAPL

# distributed research workers
./scripts/run_research.sh
```

## Architecture

```
CSV ticks ──► Python ingest ──► SQLite
                 │
                 ▼
         C++ Trading Engine
      ┌─────────┼─────────┐
   feed      signal      order
   thread    thread      thread
                 │
                 ▼
        orders + latency CSVs
                 │
                 ▼
         Python reports / SQL

Research path:
  coordinator ──► worker 0..N ──► research_stats (SQL)
```

## License

MIT
