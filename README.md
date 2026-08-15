# quant platform

small research + trading stack in c++ and python.

it covers:
- market data ingest
- multithreaded trading engine (signals + orders)
- strategy backtesting
- basic stats
- latency profiling
- sql storage
- a few worker processes for distributed research jobs

## layout

```
cpp/        trading engine + research workers
python/     ingest, signals, backtest, reports
sql/        schema
data/       sample ticks
scripts/    run helpers
```

## build

needs g++, python3, sqlite3.

```bash
make
```

that builds:
- `bin/engine` — low latency trading engine
- `bin/worker` — research worker used for distributed jobs

## setup python

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
```

## quick run

```bash
# load sample data into sqlite
python3 python/ingest.py

# run the trading engine on sample ticks
./bin/engine data/sample_ticks.csv

# run a backtest
python3 python/backtest.py

# spin up a few research workers (distributed)
./scripts/run_research.sh

# print latency summary
python3 python/latency_report.py
```

## notes

market data lives in `data/sample_ticks.csv` and gets copied into sqlite.
the engine runs feed / signal / order threads and writes latency numbers.
workers split tick ranges across processes and write stats back to the db.
