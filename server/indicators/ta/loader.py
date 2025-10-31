from __future__ import annotations
from dataclasses import dataclass

@dataclass(frozen=True)
class BackendInfo:
    name: str            # "ta-lib" or "pandas_ta"
    version: str
    ready: bool
    note: str = ""

def load_backend() -> tuple[BackendInfo, object]:
    """
    Try TA-Lib first; fall back to pandas_ta.
    Returns (BackendInfo, module_like)
    """
    # 1) Try TA-Lib (Python wrapper)
    try:
        import talib  # type: ignore
        ver = getattr(talib, "__version__", "unknown")
        return BackendInfo("ta-lib", ver, True), talib
    except Exception as e:
        ta_err = str(e)

    # 2) Fallback: pandas_ta
    try:
        import pandas as pd  # noqa: F401
        import pandas_ta as pta  # type: ignore
        ver = getattr(pta, "__version__", "unknown")
        return BackendInfo("pandas_ta", ver, True), pta
    except Exception as e:
        pta_err = str(e)

    # Neither available
    msg = (
        "No indicators backend available. Tried:\n"
        f"- TA-Lib import error: {ta_err}\n"
        f"- pandas_ta import error: {pta_err}\n"
        "Install one of:\n"
        "  pip install TA-Lib  # if wheels work for your platform\n"
        "  pip install pandas_ta\n"
        "If TA-Lib build is needed, see server/indicators/ta/install_help.md"
    )
    return BackendInfo("none", "n/a", False, msg), None
