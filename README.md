# Quant Platform

**Distributed quantitative research platform & low-latency trading engine**  
C++ · Python · SQL · Linux · Multithreading · Distributed Systems

A systems-oriented quant stack for market-data processing, strategy research, backtesting, and execution-path latency measurement. Built to mirror the core loop used in systematic trading: **ingest → signal → execute → measure → iterate**.

---

## Overview

This repository implements two tightly coupled layers:

| Layer | Focus |
|-------|--------|
| **Quant Research Platform** | Distributed tick processing, statistical aggregation, strategy backtests, and result persistence in SQL |
| **Low-Latency Trading Engine** | Multithreaded C++ runtime that ingests market data, evaluates signals, executes orders, and profiles per-stage latency |

The design emphasizes clean separation of concerns, measurable performance, and a research workflow that can scale across worker processes.

---

## Capabilities

### Market Data & Storage
- Tick-level ingest from CSV into a structured SQL schema (`ticks`, `orders`, `backtests`, `latency`, `research_stats`)
- Deterministic replay of historical prints for engine and research paths

### Low-Latency Trading Engine (C++)
- Three-stage concurrent pipeline: **feed → signal → order**
- Thread-safe queues with mutex-guarded handoff between stages
- Online signal evaluation (EMA-based mean reversion)
- High-resolution latency instrumentation (`std::chrono`) on every stage
- Export of fills and nanosecond-level timing samples for offline analysis

### Quantitative Research (C++ / Python)
- Parallel research workers that partition tick ranges across processes
- Price statistics (mean, stdev, min/max, count) computed in native C++
- Python strategy tooling for signal generation and backtest PnL / win-rate reporting
- SQL-backed experiment logging for reproducible research runs

### Observability
- Per-stage latency summaries (count, average, min, max)
- Persist latency and research outputs to SQLite for post-run inspection

---

## Architecture

```
                         ┌──────────────────────────────┐
                         │     Market Data (CSV)        │
                         └──────────────┬───────────────┘
                                        │
                    ┌───────────────────┼───────────────────┐
                    ▼                                       ▼
           Python Ingest                            C++ Trading Engine
           (SQL warehouse)                    ┌──────┼──────┐
                    │                      feed   signal  order
                    │                      thread thread  thread
                    ▼                           │
              SQLite store                      ▼
           ticks / backtests            fills + latency samples
           research_stats                       │
                    ▲                           ▼
                    │                    Python reports / SQL
           C++ Research Workers
           (partitioned jobs)
```

**Engine concurrency model:** producer/consumer stages run on dedicated threads; the feed thread publishes ticks, the signal thread evaluates alpha, and the order thread simulates execution while a shared profiler records stage timings.

**Distributed research model:** a coordinator fans out `[start, end)` tick slices to `N` worker processes; each worker writes local statistics that are aggregated into SQL.

---

## Tech Stack

| Domain | Implementation |
|--------|----------------|
| Execution / workers | C++17, pthreads, high-resolution clocks |
| Research tooling | Python 3 (stdlib) |
| Persistence | SQLite |
| Build / orchestration | Make, Bash |
| Target environment | Linux / macOS |

---

## Repository Layout

```
cpp/
  include/     engine, market data, stats, latency, worker interfaces
  src/         trading engine, research workers, entrypoints
python/        ingest, signals, backtest, latency report, SQL helpers
sql/           schema for ticks, orders, backtests, latency, research
data/          sample tick dataset
scripts/       end-to-end demo and distributed research runner
```

---

## Build

```bash
make
```

Artifacts:

- `bin/engine` — multithreaded trading engine with latency profiling
- `bin/worker` — research worker for distributed statistical jobs

Requires: `g++` (C++17), Python 3, SQLite 3.

---

## Run

Full pipeline (ingest → engine → backtest → distributed research):

```bash
./scripts/run_platform.sh
```

Individual stages:

```bash
python3 python/ingest.py
./bin/engine data/sample_ticks.csv
python3 python/store_results.py orders
python3 python/latency_report.py
python3 python/backtest.py AAPL
./scripts/run_research.sh
```

---

## Design Notes

- **Latency as a first-class metric** — every hot-path stage is timed in nanoseconds and persisted for analysis
- **Research / production split** — Python owns experimentation and reporting; C++ owns the performance-sensitive path
- **Horizontal research fan-out** — workloads shard by tick range across processes, similar in spirit to batch research grids
- **Reproducibility** — schema-backed storage of orders, backtests, latency, and worker statistics

---

## License

MIT
