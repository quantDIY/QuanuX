# QuanuX Institutional Reference: Execution Exposure Bounds

This document outlines the absolute exposure boundaries mapped to the Ritchie Finite State Machine (FSM). The QuanuX-Spreader cannot inherently violate these constraints as they are enforced asynchronously by the QuanuX-Sentinel running on an isolated hardware port.

## 1. STATE_VOID (0)
- **Net Position Exposure:** 0
- **Order Dispatch Capacity:** Locked.
- **System Memory:** Pinned L3 Shared Region only. Heap allocations flag a kernel breach.

## 2. STATE_VIGIL (1)
- **Net Position Exposure:** 0
- **Order Dispatch Capacity:** Flat Limit only (Can dispatch initial establishing clips).
- **Interlock Status:** Clear (0).

## 3. STATE_ENGAGED (2)
- **Net Position Exposure:** `0 < ABS(current_position) <= position_limit_max`
- **Order Dispatch Capacity:** Active.
- *Assertion Check*: If a tick evaluates beyond the `max` tolerance, the Sentinel will toggle the bitmask and drop the Spreader to `STATE_HEDGE`.

## 4. STATE_HEDGE (3)
- **Net Position Exposure:** `ABS(current_position) <= position_limit_max`
- **Order Dispatch Capacity:** RESTRICTED to risk-reducing vectors only.
- *Assertion Check*: If the Spreader attempts to add size during a Hedge operation, the 59ns loop triggers `STATE_HALT`.

## 5. STATE_HALT (4)
- **Net Position Exposure:** Frozen (Liquidating off-exchange via Clearing Firm protocols).
- **Order Dispatch Capacity:** Full hardware kill-switch. DMA NATS pipe is immediately severed.

## 6. STATE_RECOVERY (5)
- **Net Position Exposure:** Inherited from Warm Restart Partial (`.qlog` memory).
- **Order Dispatch Capacity:** Synchronization check before transitioning back to `STATE_VIGIL`.

## The "Shakespeare in the Sand" Governance
Refer to the `IMMORTAL_RATIONALE.md` document for the cryptographic governance boundaries regarding Git deployment tracking and the Replay simulation requirements.
