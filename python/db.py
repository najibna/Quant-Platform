"""db helpers for the platform"""

import os
import sqlite3

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
DB_PATH = os.path.join(ROOT, "data", "quant.db")
SCHEMA_PATH = os.path.join(ROOT, "sql", "schema.sql")


def connect():
    # make sure data folder exists
    os.makedirs(os.path.dirname(DB_PATH), exist_ok=True)
    conn = sqlite3.connect(DB_PATH)
    conn.row_factory = sqlite3.Row
    return conn


def init_db():
    # create tables if they are missing
    conn = connect()
    with open(SCHEMA_PATH, "r", encoding="utf-8") as f:
        conn.executescript(f.read())
    conn.commit()
    conn.close()
