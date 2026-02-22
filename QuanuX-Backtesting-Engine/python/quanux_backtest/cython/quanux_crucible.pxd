# distutils: language = c++

from libcpp.string cimport string
from libcpp.vector cimport vector
from libc.stdint cimport uint64_t, uint32_t, int64_t, int32_t, uint8_t

cdef extern from "engine/metrics/PerformanceAnalyzer.h" namespace "quanux::engine::metrics":
    cdef struct CrucibleTrade:
        double entryPrice
        double exitPrice
        double profit
        double profitPct
        double risk
        int32_t durationBars
        uint8_t isLong
        double mae
        double mfe
        uint32_t queue_position_at_entry
        uint32_t size
        double latency_slippage_bps
        uint64_t entry_time_ns
        uint64_t exit_time_ns

    cdef struct Metrics:
        double netProfit
        double grossProfit
        double grossLoss
        double profitFactor
        double cagr
        double winRate
        double expectancy
        double maxDrawdownPct
        double avgDrawdownPct
        double ulcerIndex
        double annualizedVol
        double sharpeRatio
        double sortinoRatio
        double calmarRatio
        double omegaRatio
        double informationRatio
        double sqn

    cdef cppclass PerformanceAnalyzer:
        PerformanceAnalyzer(double startingEquity, double riskFreeRate) except +
        void addTrade(const CrucibleTrade &t)
        void setEquityCurve(const vector[double] &equity)
        void updateEquity(double currentEquity)
        Metrics calculateMetrics(int periodsPerYear)

cdef extern from "engine/DuckDBFeeder.h" namespace "quanux::engine":
    cdef cppclass DuckDBFeeder:
        DuckDBFeeder(const string &db_path) except +
        void init_appenders()
        void append_trades(const string& strategy_id, const vector[CrucibleTrade]& trades)
        string get_metrics_json(const string& strategy_id)
        string get_metrics_json_advanced(const string& strategy_id, int mc_iterations)

cdef extern from "engine/BacktestRunner.h" namespace "quanux::engine":
    cdef struct BacktestConfig:
        bint enable_nats
        string nats_url

    cdef cppclass BacktestRunner:
        BacktestRunner() except +
        void run(const BacktestConfig &config)
