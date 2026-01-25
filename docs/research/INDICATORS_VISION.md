# Quanux Indicators Vision (Issue #14)

## Executive Summary
**TA-Lib** is the industry standard but shows its age (C89, stateful, heap allocations).
**Quanux Indicators** (`libquanux-indicators`) is a C++20 standard for HFT/ML, designed to be **Community Extensible**.

---

## 1. Core Philosophy: "The Compute Graph"
Indicators are nodes in a **Directed Acyclic Graph (DAG)**.
*   **Auto-ML**: Tweak one parameter, only recompute downstream nodes.
*   **Lazy Evaluation**: C++20 pipes `price | sma<10> | rsi<14>`.

---

## 2. Auction Market Theory (Dalton & Order Flow)
*Reference: "Mind Over Markets" by James Dalton.*

### Market Profile (TPO)
*   **TPO Units**: 30-minute letters (A, B, C...) mapping price interactions over time.
*   **Structure Detection**:
    *   **Initial Balance (IB)**: First hour range.
    *   **Extension**: Breaking the IB.
    *   **Buying/Selling Tails (Excess)**: Single TPO prints at extremes indicating rejection.
    *   **Poor High/Low**: Lack of excess, suggesting unfinished business.
*   **Balance vs Imbalance**: Detecting when the market is "Accepting" value vs "Exploring".

### Volume Profile (VP)
*   **Micro-Composite**: Merging profiles dynamically (e.g., "Merge the last 3 days of balance").
*   **VWAP Bands**: 1/2/3 StdDev bands anchored to session open or swing pivots.

---

## 3. Extensibility & The "Indicator Registry"
*Goal: infinite number of indicators via UI/git.*

### The "Git-as-Registry" Model
Indicators are just C++ header files in a repo.
1.  **Community Repo**: `github.com/quantDIY/quanux-indicators-contrib`
2.  **Local Install**: `quanuxctl indicators install <repo>`

### Workflow: `quanuxctl update indicators`
1.  **Pull**: Git pulls the latest `.h` files into `server/indicators/include/contrib`.
2.  **JIT Compile**: Runs a fast `cmake --build` to re-link generic bindings.
3.  **Hot Load**: The Python strategy builder sees the new indicators immediately.

---

## 4. Advanced Systems (Kaufman & Chaos)
*   **Adaptive Moving Average (AMA)**: Adjusts speed based on market noise.
*   **Efficiency Ratio (ER)**: Trend strength metric.
*   **Fractal Dimension**: Hurst Exponent.

---

## Roadmap
1.  **Core Interface**: `Indicator<T>`.
2.  **Auction Set**: TPO, Tails, VP, VWAP.
3.  **Registry Tooling**: `quanuxctl` commands to manage the library.
