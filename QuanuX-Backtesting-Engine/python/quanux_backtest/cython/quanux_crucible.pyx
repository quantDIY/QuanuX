# distutils: language = c++

cimport quanux_crucible
from libcpp.string cimport string
from libcpp.vector cimport vector

cdef class PyCrucibleTrade:
    cdef quanux_crucible.CrucibleTrade _trade

    def __cinit__(self):
        # Initialize Memory
        self._trade.entryPrice = 0.0
        self._trade.exitPrice = 0.0
        self._trade.profit = 0.0
        self._trade.profitPct = 0.0
        self._trade.risk = 0.0
        self._trade.durationBars = 0
        self._trade.isLong = 0
        self._trade.mae = 0.0
        self._trade.mfe = 0.0
        self._trade.queue_position_at_entry = 0
        self._trade.size = 0
        self._trade.latency_slippage_bps = 0.0
        self._trade.entry_time_ns = 0
        self._trade.exit_time_ns = 0

    @property
    def entryPrice(self): return self._trade.entryPrice
    @entryPrice.setter
    def entryPrice(self, double value): self._trade.entryPrice = value

    @property
    def exitPrice(self): return self._trade.exitPrice
    @exitPrice.setter
    def exitPrice(self, double value): self._trade.exitPrice = value

    @property
    def profit(self): return self._trade.profit
    @profit.setter
    def profit(self, double value): self._trade.profit = value

    @property
    def profitPct(self): return self._trade.profitPct
    @profitPct.setter
    def profitPct(self, double value): self._trade.profitPct = value

    @property
    def risk(self): return self._trade.risk
    @risk.setter
    def risk(self, double value): self._trade.risk = value

    @property
    def durationBars(self): return self._trade.durationBars
    @durationBars.setter
    def durationBars(self, int value): self._trade.durationBars = value

    @property
    def isLong(self): return self._trade.isLong
    @isLong.setter
    def isLong(self, int value): self._trade.isLong = value

    @property
    def mae(self): return self._trade.mae
    @mae.setter
    def mae(self, double value): self._trade.mae = value

    @property
    def mfe(self): return self._trade.mfe
    @mfe.setter
    def mfe(self, double value): self._trade.mfe = value

    @property
    def queue_position_at_entry(self): return self._trade.queue_position_at_entry
    @queue_position_at_entry.setter
    def queue_position_at_entry(self, int value): self._trade.queue_position_at_entry = value

    @property
    def size(self): return self._trade.size
    @size.setter
    def size(self, int value): self._trade.size = value

    @property
    def latency_slippage_bps(self): return self._trade.latency_slippage_bps
    @latency_slippage_bps.setter
    def latency_slippage_bps(self, double value): self._trade.latency_slippage_bps = value

    @property
    def entry_time_ns(self): return self._trade.entry_time_ns
    @entry_time_ns.setter
    def entry_time_ns(self, int value): self._trade.entry_time_ns = value

    @property
    def exit_time_ns(self): return self._trade.exit_time_ns
    @exit_time_ns.setter
    def exit_time_ns(self, int value): self._trade.exit_time_ns = value


cdef class PyPerformanceAnalyzer:
    cdef quanux_crucible.PerformanceAnalyzer* _analyzer

    def __cinit__(self, double startingEquity=10000.0, double riskFreeRate=0.0):
        self._analyzer = new quanux_crucible.PerformanceAnalyzer(startingEquity, riskFreeRate)

    def __dealloc__(self):
        if self._analyzer != NULL:
            del self._analyzer

    def addTrade(self, PyCrucibleTrade trade):
        self._analyzer.addTrade(trade._trade)

    def updateEquity(self, double currentEquity):
        self._analyzer.updateEquity(currentEquity)

    def calculateMetrics(self, int periodsPerYear=252):
        cdef quanux_crucible.Metrics _m = self._analyzer.calculateMetrics(periodsPerYear)
        return {
            "netProfit": _m.netProfit,
            "grossProfit": _m.grossProfit,
            "grossLoss": _m.grossLoss,
            "profitFactor": _m.profitFactor,
            "cagr": _m.cagr,
            "winRate": _m.winRate,
            "expectancy": _m.expectancy,
            "maxDrawdownPct": _m.maxDrawdownPct,
            "avgDrawdownPct": _m.avgDrawdownPct,
            "ulcerIndex": _m.ulcerIndex,
            "annualizedVol": _m.annualizedVol,
            "sharpeRatio": _m.sharpeRatio,
            "sortinoRatio": _m.sortinoRatio,
            "calmarRatio": _m.calmarRatio,
            "omegaRatio": _m.omegaRatio,
            "informationRatio": _m.informationRatio,
            "sqn": _m.sqn,
        }

cdef class PyDuckDBFeeder:
    cdef quanux_crucible.DuckDBFeeder* _feeder

    def __cinit__(self, str db_path=":memory:"):
        self._feeder = new quanux_crucible.DuckDBFeeder(db_path.encode('utf-8'))

    def __dealloc__(self):
        if self._feeder != NULL:
            del self._feeder

    def init_appenders(self):
        self._feeder.init_appenders()

    def append_trades(self, str strategy_id, list trades):
        cdef vector[quanux_crucible.CrucibleTrade] c_trades
        cdef PyCrucibleTrade t
        for t in trades:
            c_trades.push_back(t._trade)
        self._feeder.append_trades(strategy_id.encode('utf-8'), c_trades)

    def get_metrics_json(self, str strategy_id):
        return self._feeder.get_metrics_json(strategy_id.encode('utf-8')).decode('utf-8')

    def get_metrics_json_advanced(self, str strategy_id, int mc_iterations = 1000):
        return self._feeder.get_metrics_json_advanced(strategy_id.encode('utf-8'), mc_iterations).decode('utf-8')

cdef class PyBacktestRunner:
    cdef quanux_crucible.BacktestRunner* _runner

    def __cinit__(self):
        self._runner = new quanux_crucible.BacktestRunner()

    def __dealloc__(self):
        if self._runner != NULL:
            del self._runner

    def run(self, bint enable_nats=False, str nats_url="nats://localhost:4222"):
        cdef quanux_crucible.BacktestConfig cfg
        cfg.enable_nats = enable_nats
        cfg.nats_url = nats_url.encode('utf-8')
        self._runner.run(cfg)
