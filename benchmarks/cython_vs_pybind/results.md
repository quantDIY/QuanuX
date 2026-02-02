# Cython vs Pybind11 Benchmark

We investigated whether switching from Pybind11 to Cython for C++ wrappers would yield significant performance gains.

## The Benchmark
We created a micro-benchmark comparing a simple C++ `add(int, int)` function exposed via:
1.  **Pybind11**: The current binding technology.
2.  **Cython**: The proposed replacement.

## Results (10 Million calls)

| Implementation | Time (s) | Throughput (M/ops) | Overhead per call |
| :--- | :--- | :--- | :--- |
| **Pure Python** | 0.43s | 23.06 | ~43ns |
| **Pybind11** | 2.85s | 3.50 | ~285ns |
| **Cython** | 0.41s | 24.19 | ~41ns |

**Conclusion**: Cython is **~6.9x faster** than Pybind11 for individual function calls.

## Analysis: Does it matter?

It depends entirely on **call frequency**.

### Case A: Batch Processing (Low Impact)
If you pass a list of 10,000 prices to C++ once:
`sma = indicators.compute_sma(prices, 10)`
-   Pybind11 overhead: 285ns (negligible).
-   Cython overhead: 41ns (negligible).
-   **Verdict**: No meaningful difference.

### Case B: per-Tick Updates (High Impact)
The current `process(price)` API pattern in `indicators` suggests updating state tick-by-tick.
If you track **50 indicators** processing **100,000 ticks/sec**:
-   Total calls: 5,000,000 / sec.
-   **Pybind11 cost**: 5M * 285ns = **1.425 seconds** (Slower than real-time!)
-   **Cython cost**: 5M * 41ns = **0.205 seconds** (20% CPU load).

## Recommendation
**YES**, switching to Cython matters significantly for the `indicators` library because it relies on fine-grained `process()` calls. Using Pybind11 for this pattern puts a hard ceiling on the number of indicators/strategies you can run in real-time.
