"""read engine latency csv and store a short summary in sql"""

import csv
import os
import sys
import uuid
from collections import defaultdict

sys.path.insert(0, os.path.dirname(__file__))

from db import ROOT, connect, init_db


def load_latency_csv(path=None):
    if path is None:
        path = os.path.join(ROOT, "data", "processed", "latency.csv")
    if not os.path.exists(path):
        print(f"missing {path}, run the engine first")
        return []

    rows = []
    with open(path, "r", encoding="utf-8") as f:
        for r in csv.DictReader(f):
            rows.append((r["stage"], int(r["nanos"])))
    return rows


def report(path=None):
    samples = load_latency_csv(path)
    if not samples:
        return

    init_db()
    run_id = uuid.uuid4().hex[:8]
    conn = connect()

    buckets = defaultdict(list)
    for stage, nanos in samples:
        buckets[stage].append(nanos)
        conn.execute(
            "insert into latency (stage, nanos, run_id) values (?, ?, ?)",
            (stage, nanos, run_id),
        )
    conn.commit()

    print(f"latency report run_id={run_id}")
    for stage, vals in sorted(buckets.items()):
        avg = sum(vals) / len(vals)
        mn = min(vals)
        mx = max(vals)
        print(f"  {stage}: n={len(vals)} avg={avg:.1f}ns min={mn} max={mx}")

    conn.close()


if __name__ == "__main__":
    report(sys.argv[1] if len(sys.argv) > 1 else None)
