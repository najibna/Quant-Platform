"""run a tiny mean reversion backtest and store results in sql"""

import os
import sys
from datetime import datetime, timezone

sys.path.insert(0, os.path.dirname(__file__))

from db import connect, init_db
from signals import mean_reversion_signals


def load_prices(symbol="AAPL"):
    init_db()
    conn = connect()
    rows = conn.execute(
        "select price from ticks where symbol = ? order by ts",
        (symbol,),
    ).fetchall()
    conn.close()
    return [r["price"] for r in rows]


def run_backtest(symbol="AAPL", size=10):
    prices = load_prices(symbol)
    if len(prices) < 2:
        print("not enough prices, run ingest first")
        return

    signals = mean_reversion_signals(prices)
    cash = 0.0
    pos = 0
    trades = 0
    wins = 0
    last_entry = None

    for i in range(len(prices)):
        sig = signals[i]
        px = prices[i]

        if sig == 1 and pos <= 0:
            # cover short then go long
            if pos < 0:
                cash -= abs(pos) * px
                trades += 1
                if last_entry is not None and px < last_entry:
                    wins += 1
            pos = size
            cash -= size * px
            last_entry = px
            trades += 1
        elif sig == -1 and pos >= 0:
            if pos > 0:
                cash += pos * px
                trades += 1
                if last_entry is not None and px > last_entry:
                    wins += 1
            pos = -size
            cash += size * px
            last_entry = px
            trades += 1

    # flatten at the end
    if pos != 0:
        cash += pos * prices[-1]
        trades += 1
        pos = 0

    pnl = cash
    win_rate = (wins / trades) if trades else 0.0

    conn = connect()
    conn.execute(
        """
        insert into backtests (strategy, symbol, pnl, trades, win_rate, created_at)
        values (?, ?, ?, ?, ?, ?)
        """,
        (
            "mean_reversion",
            symbol,
            pnl,
            trades,
            win_rate,
            datetime.now(timezone.utc).isoformat(),
        ),
    )
    conn.commit()
    conn.close()

    print(f"backtest {symbol}: pnl={pnl:.2f} trades={trades} win_rate={win_rate:.2f}")


if __name__ == "__main__":
    sym = sys.argv[1] if len(sys.argv) > 1 else "AAPL"
    run_backtest(sym)
