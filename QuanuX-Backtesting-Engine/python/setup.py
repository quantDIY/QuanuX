from setuptools import setup, find_packages

setup(
    name="quanux_backtest",
    version="0.1.0",
    packages=find_packages(),
    install_requires=[
        "pandas",
        "numpy"
    ],
    description="Python wrapper for QuanuX C++ Backtesting Metrics",
    author="QuanuX Team",
)
