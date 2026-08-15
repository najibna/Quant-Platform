-- simple schema for research + trading stuff

create table if not exists ticks (
    id integer primary key autoincrement,
    symbol text not null,
    ts real not null,
    price real not null,
    size integer not null
);

create table if not exists orders (
    id integer primary key autoincrement,
    symbol text not null,
    side text not null,
    price real not null,
    size integer not null,
    ts real not null,
    status text not null
);

create table if not exists backtests (
    id integer primary key autoincrement,
    strategy text not null,
    symbol text not null,
    pnl real not null,
    trades integer not null,
    win_rate real not null,
    created_at text not null
);

create table if not exists latency (
    id integer primary key autoincrement,
    stage text not null,
    nanos integer not null,
    run_id text not null
);

create table if not exists research_stats (
    id integer primary key autoincrement,
    worker_id integer not null,
    symbol text not null,
    mean_price real not null,
    std_price real not null,
    tick_count integer not null,
    elapsed_ms real not null
);
