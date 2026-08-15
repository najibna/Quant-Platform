#!/usr/bin/env bash
# one shot demo of the whole platform

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

mkdir -p data/processed logs
make

echo "== ingest =="
python3 python/ingest.py

echo "== engine =="
./bin/engine data/sample_ticks.csv

echo "== store orders + latency =="
python3 python/store_results.py orders
python3 python/latency_report.py

echo "== backtest =="
python3 python/backtest.py AAPL
python3 python/backtest.py MSFT

echo "== distributed research =="
./scripts/run_research.sh

echo "done"
