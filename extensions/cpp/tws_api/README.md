# TWS API Adapter

**Native C++ Integration for Interactive Brokers**

> [!WARNING]
> This extension requires **Manual Injection** of the proprietary SDK.

## Setup Instructions
1.  **Locate SDK**: You must have the `twsapi_macunix` folder (containing `IBJts`).
2.  **Integrate**: Run the integration command (once available) or manually copy the headers:
    -   Copy `twsapi_macunix/IBJts/source/cppclient/client/*.h` to `extensions/cpp/tws_api/include/`.
3.  **Build**: Use CMake to link against this adapter.

## Features
-   Fast Market Data (Top of Book)
-   Retail Order Types (Algos)
-   No FIX Session layer overhead.
