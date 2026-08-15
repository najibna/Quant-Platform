"""simple signal helpers used by the backtester"""


def ema(values, alpha=0.2):
    # exponential moving average
    if not values:
        return []
    out = [values[0]]
    for v in values[1:]:
        out.append(alpha * v + (1.0 - alpha) * out[-1])
    return out


def mean_reversion_signals(prices, alpha=0.2, band=0.05):
    # +1 buy, -1 sell, 0 hold
    avg = ema(prices, alpha)
    signals = []
    for p, a in zip(prices, avg):
        diff = p - a
        if diff < -band:
            signals.append(1)
        elif diff > band:
            signals.append(-1)
        else:
            signals.append(0)
    return signals
