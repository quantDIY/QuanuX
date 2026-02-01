# Interactive Brokers FIX Specification Summary

**Version**: 2025-001 (Oct 27, 2025)
**Supported FIX Versions**: 4.0 - 4.4 (**Preferred: 4.2**)

## Connectivity & Authentication
-   **Methods**:
    1.  **IBKR Gateway / TWS**: Connects via localhost (or networked) to a running IBKR Gateway instance. This is the standard "Retail/API" method.
    2.  **Direct (Cross-Connect/VPN/Extranet)**: For high-volume/institutional clients.
-   **Credentials**:
    -   `SenderCompID` (49): Your IBKR Username (default).
    -   `TargetCompID` (56): "IB" (default) or "IBKR" (older).
    -   `SenderSubID` (50): Optional.
-   **Network**:
    -   SSL Tunneling is used by the IBKR Gateway.
    -   Direct connections use IP whitelisting + sequences.
-   **Session**:
    -   **Reset**: Nightly (~12:30 AM ET) or Weekly (Saturday).
    -   **Sequence Numbers**: 1 to 999999. Resets required if >999999.
    -   **Heartbeat**: 30s recommended.

## Key Message types
-   **Logon (A)**: `EncryptMethod=0` (None), `HeartBtInt=30`.
-   **Order Entry (D)**:
    -   `ClOrdID` (11): Unique ID.
    -   `Account` (1): Required.
    -   `ExDestination` (100): "SMART" is the primary router. Requires `Currency` (15) or `SecurityExchange` (207).
    -   `SecurityType` (167): "STK" (Stock), "OPT" (Option), "FUT" (Future), "FOP" (Future Option), "CASH" (Forex), "CRYPTO".
    -   `TimeInForce` (59): Standard (DAY, GTC, IOC).
-   **Custom Tags**:
    -   `6035` (IBKRLocalSymbol)
    -   `6010` (Order Reference)
    -   `6122` (Order Capacity - alt)
    -   See Appendix B/C for full list.

## Recent Features (2024-2025)
-   **Crypto Support**: via FIX.
-   **Fractional Trading**: Supported.
-   **Algorithm Support**: IBKR Algos (Adaptive, Accumulate/Distribute) supported via specific tags (Types often mapped to custom values).
-   **Snap to Market/Midpoint** orders.

## Implementation Notes for QuanuX
-   **Target**: FIX 4.2.
-   **Config**: Needs to point to IBKR Gateway port (default typically 4001 or 7496, but FIX might use a specific one configured in Gateway settings).
-   **Dictionary**: Standard FIX 4.2 xml, potentially patched with IBKR custom tags (start with standard, add custom fields if using DataDictionary validation).
