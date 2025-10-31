from __future__ import annotations
import pandas as pd
from .loader import load_backend

_BACKEND_INFO, _MOD = load_backend()

def _need_backend():
    if not _BACKEND_INFO.ready or _MOD is None:
        raise RuntimeError(_BACKEND_INFO.note or "Indicators backend not ready")

def sma(close: pd.Series, length: int = 20) -> pd.Series:
    _need_backend()
    if _BACKEND_INFO.name == "ta-lib":
        return pd.Series(_MOD.SMA(close.values, timeperiod=length), index=close.index, name=f"SMA_{length}")
    # pandas_ta
    return _MOD.sma(close=close, length=length).rename(f"SMA_{length}")

def ema(close: pd.Series, length: int = 20) -> pd.Series:
    _need_backend()
    if _BACKEND_INFO.name == "ta-lib":
        return pd.Series(_MOD.EMA(close.values, timeperiod=length), index=close.index, name=f"EMA_{length}")
    return _MOD.ema(close=close, length=length).rename(f"EMA_{length}")

def rsi(close: pd.Series, length: int = 14) -> pd.Series:
    _need_backend()
    if _BACKEND_INFO.name == "ta-lib":
        return pd.Series(_MOD.RSI(close.values, timeperiod=length), index=close.index, name=f"RSI_{length}")
    return _MOD.rsi(close=close, length=length).rename(f"RSI_{length}")

def macd(close: pd.Series, fast: int = 12, slow: int = 26, signal: int = 9) -> pd.DataFrame:
    _need_backend()
    if _BACKEND_INFO.name == "ta-lib":
        macd_, signal_, hist_ = _MOD.MACD(close.values, fastperiod=fast, slowperiod=slow, signalperiod=signal)
        return pd.DataFrame(
            {"MACD": macd_, "MACD_signal": signal_, "MACD_hist": hist_}, index=close.index
        )
    out = _MOD.macd(close=close, fast=fast, slow=slow, signal=signal)
    # pandas_ta returns columns like MACD_12_26_9, MACDh_..., MACDs_...
    return out.rename(columns={
        out.columns[0]: "MACD",
        out.columns[1]: "MACD_hist",
        out.columns[2]: "MACD_signal",
    })
