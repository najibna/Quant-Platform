#!/usr/bin/env bash
# fan out research jobs across a few worker processes

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

mkdir -p data/processed logs bin
make -s

CSV="data/sample_ticks.csv"
OUT="data/processed/research.csv"
rm -f "$OUT"

# count data lines (skip header)
TOTAL=$(tail -n +2 "$CSV" | wc -l | tr -d ' ')
WORKERS=4
CHUNK=$(( (TOTAL + WORKERS - 1) / WORKERS ))

echo "starting $WORKERS workers over $TOTAL ticks"

pids=()
i=0
start=0
while [ "$i" -lt "$WORKERS" ]; do
  end=$(( start + CHUNK ))
  if [ "$end" -gt "$TOTAL" ]; then
    end=$TOTAL
  fi

  # alternate symbols so workers do slightly different jobs
  if [ $(( i % 2 )) -eq 0 ]; then
    sym="AAPL"
  else
    sym="MSFT"
  fi

  ./bin/worker "$i" "$CSV" "$sym" "$start" "$end" "$OUT" \
    > "logs/worker_${i}.log" 2>&1 &
  pids+=($!)

  start=$end
  i=$(( i + 1 ))
done

# wait for everyone
for pid in "${pids[@]}"; do
  wait "$pid"
done

echo "workers done, results in $OUT"
python3 python/store_results.py research
