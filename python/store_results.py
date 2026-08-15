"""save engine orders and research worker output into sqlite"""

import csv
import os
import sys

sys.path.insert(0, os.path.dirname(__file__))

from db import ROOT, connect, init_db


def store_orders(path=None):
    if path is None:
        path = os.path.join(ROOT, "data", "processed", "orders.csv")
    if not os.path.exists(path):
        print(f"no orders file at {path}")
        return 0

    init_db()
    conn = connect()
    with open(path, "r", encoding="utf-8") as f:
        rows = [
            (r["symbol"], r["side"], float(r["price"]), int(r["size"]), float(r["ts"]), r["status"])
            for r in csv.DictReader(f)
        ]
    conn.executemany(
        "insert into orders (symbol, side, price, size, ts, status) values (?, ?, ?, ?, ?, ?)",
        rows,
    )
    conn.commit()
    conn.close()
    print(f"stored {len(rows)} orders")
    return len(rows)


def store_research(path=None):
    if path is None:
        path = os.path.join(ROOT, "data", "processed", "research.csv")
    if not os.path.exists(path):
        print(f"no research file at {path}")
        return 0

    init_db()
    conn = connect()
    with open(path, "r", encoding="utf-8") as f:
        # worker_id,symbol,mean,std,count,elapsed_ms
        rows = []
        for line in f:
            line = line.strip()
            if not line:
                continue
            parts = line.split(",")
            if len(parts) < 6:
                continue
            rows.append(
                (
                    int(parts[0]),
                    parts[1],
                    float(parts[2]),
                    float(parts[3]),
                    int(parts[4]),
                    float(parts[5]),
                )
            )
    conn.executemany(
        """
        insert into research_stats
        (worker_id, symbol, mean_price, std_price, tick_count, elapsed_ms)
        values (?, ?, ?, ?, ?, ?)
        """,
        rows,
    )
    conn.commit()
    conn.close()
    print(f"stored {len(rows)} research rows")
    return len(rows)


if __name__ == "__main__":
    what = sys.argv[1] if len(sys.argv) > 1 else "all"
    if what in ("orders", "all"):
        store_orders()
    if what in ("research", "all"):
        store_research()
