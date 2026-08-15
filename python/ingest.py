"""load sample ticks into sqlite"""

import csv
import os
import sys

# allow running from repo root
sys.path.insert(0, os.path.dirname(__file__))

from db import ROOT, connect, init_db


def ingest(csv_path=None):
    if csv_path is None:
        csv_path = os.path.join(ROOT, "data", "sample_ticks.csv")

    init_db()
    conn = connect()
    cur = conn.cursor()

    # wipe old ticks so reruns stay clean
    cur.execute("delete from ticks")

    with open(csv_path, "r", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        rows = [
            (r["symbol"], float(r["ts"]), float(r["price"]), int(r["size"]))
            for r in reader
        ]

    cur.executemany(
        "insert into ticks (symbol, ts, price, size) values (?, ?, ?, ?)",
        rows,
    )
    conn.commit()
    print(f"ingested {len(rows)} ticks into {os.path.join(ROOT, 'data', 'quant.db')}")
    conn.close()


if __name__ == "__main__":
    path = sys.argv[1] if len(sys.argv) > 1 else None
    ingest(path)
