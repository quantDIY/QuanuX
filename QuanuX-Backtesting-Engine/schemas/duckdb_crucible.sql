-- QuanuX Crucible: DuckDB Schema Initialization
-- These tables define the state of a backtest run and are instantiated locally for each strategy version.

CREATE TABLE IF NOT EXISTS crucible_performance (
    strategy_id VARCHAR PRIMARY KEY,
    start_time TIMESTAMP,
    end_time TIMESTAMP,
    -- Profitability
    total_return DOUBLE,
    cagr DOUBLE,
    net_profit DOUBLE,
    gross_profit DOUBLE,
    gross_loss DOUBLE,
    avg_profit_loss_per_trade DOUBLE,
    profit_factor DOUBLE,
    expectancy DOUBLE,
    -- Risk
    max_drawdown_percent DOUBLE,
    max_drawdown_dollar DOUBLE,
    avg_drawdown DOUBLE,
    volatility DOUBLE,
    value_at_risk DOUBLE,
    loss_std_dev DOUBLE,
    recovery_time_ms BIGINT,
    -- Risk-Adjusted
    sharpe_ratio DOUBLE,
    sortino_ratio DOUBLE,
    calmar_ratio DOUBLE,
    reward_to_risk_ratio DOUBLE,
    information_ratio DOUBLE,
    -- Trade Statistics
    total_trades INTEGER,
    win_rate DOUBLE,
    winning_trades INTEGER,
    losing_trades INTEGER,
    avg_win DOUBLE,
    avg_loss DOUBLE,
    max_consecutive_wins INTEGER,
    max_consecutive_losses INTEGER,
    avg_trade_duration_ms BIGINT,
    -- Market Context
    alpha DOUBLE,
    beta DOUBLE,
    turnover_rate DOUBLE,
    time_in_market_percent DOUBLE,
    total_slippage_costs DOUBLE,
    total_commission_costs DOUBLE
);

CREATE TABLE IF NOT EXISTS crucible_trades (
    trade_id BIGINT PRIMARY KEY,
    strategy_id VARCHAR,
    entry_time TIMESTAMP,
    exit_time TIMESTAMP,
    direction VARCHAR(10), -- 'LONG' or 'SHORT'
    size INTEGER,
    fill_price DOUBLE,
    exit_price DOUBLE,
    latency_slippage_bps DOUBLE,
    mae DOUBLE,  -- Maximum Adverse Excursion
    mfe DOUBLE,  -- Maximum Favorable Excursion
    queue_position_entry INTEGER,
    net_pnl DOUBLE,
    FOREIGN KEY(strategy_id) REFERENCES crucible_performance(strategy_id)
);

CREATE TABLE IF NOT EXISTS crucible_events (
    event_id BIGINT PRIMARY KEY,
    strategy_id VARCHAR,
    event_time TIMESTAMP,
    event_type VARCHAR(50), -- 'SIGNAL', 'ORDER_SENT', 'STOP_LOSS_HIT', etc.
    details JSON,           -- Arbitrary JSON payload for indicator values or ML features
    FOREIGN KEY(strategy_id) REFERENCES crucible_performance(strategy_id)
);
