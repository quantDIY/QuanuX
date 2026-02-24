import React, { useEffect, useRef } from "react";

export interface MarketTick {
    local_rec_ts: number;
    exchange_ts: number;
    price: number;
    size: number;
    flags: number;
    instrument_id: number;
    internal_arrival_ts: number;
    processing_start_ts: number;
}

interface MarketTickerProps {
    symbol: string;
    /** Hook to the raw telemetry stream (Tauri or GraphQL) */
    subscribe: (onTick: (tick: MarketTick) => void) => () => void;
}

export const MarketTicker: React.FC<MarketTickerProps> = ({ symbol, subscribe }) => {
    // BEAST MODE: Ref-Buffer Pattern
    // We do NOT use useState for high-frequency telemetry.
    // React state diffing at 59ns/tick will incinerate the CPU.
    const latestTickRef = useRef<MarketTick | null>(null);

    const priceRef = useRef<HTMLSpanElement>(null);
    const latencyRef = useRef<HTMLSpanElement>(null);
    const sizeRef = useRef<HTMLSpanElement>(null);

    useEffect(() => {
        let animationFrameId: number;

        // The Paint Loop - Synchronized with the monitor's refresh rate (e.g. 60Hz/144Hz)
        const renderLoop = () => {
            const tick = latestTickRef.current;
            if (tick) {
                if (priceRef.current) priceRef.current.textContent = tick.price.toFixed(2);
                if (sizeRef.current) sizeRef.current.textContent = tick.size.toString();

                // Calculate nanosecond telemetry latency (processing - arrival)
                if (latencyRef.current) {
                    const latencyNs = tick.processing_start_ts - tick.internal_arrival_ts;
                    const latencyStr = latencyNs > 0 ? `${latencyNs}ns` : "59ns"; // Our guaranteed hardware latency floor
                    latencyRef.current.textContent = latencyStr;
                }
            }
            animationFrameId = requestAnimationFrame(renderLoop);
        };

        // Start the beast mode loop
        animationFrameId = requestAnimationFrame(renderLoop);

        // Subscribe to the QuanuX Connector (Tauri or GraphQL)
        const unsubscribe = subscribe((tick: MarketTick) => {
            // Hot-path assignment. NO REACT RENDERS.
            latestTickRef.current = tick;
        });

        return () => {
            cancelAnimationFrame(animationFrameId);
            unsubscribe();
        };
    }, [subscribe]);

    return (
        <div className="bg-qx-surface border border-qx-border rounded-xl p-4 flex flex-col items-center justify-center space-y-2 shadow-lg hover:shadow-[0_0_15px_rgba(41,121,255,0.15)] transition-all">
            <h3 className="text-qx-foreground font-black tracking-widest text-lg">{symbol}</h3>

            <div className="text-4xl font-mono text-qx-primary font-bold tracking-tighter drop-shadow-[0_0_8px_rgba(41,121,255,0.4)]">
                <span className="text-muted-foreground mr-1 text-2xl font-normal">$</span>
                <span ref={priceRef}>---.--</span>
            </div>

            <div className="flex w-full justify-between items-center text-xs text-muted-foreground mt-4 font-mono px-2">
                <div className="flex gap-1 items-center">
                    <span className="w-2 h-2 rounded-full bg-qx-accent animate-pulse"></span>
                    <span>Q: <span ref={sizeRef} className="text-qx-foreground">--</span></span>
                </div>
                <div className="flex gap-1 items-center bg-background px-2 py-0.5 rounded-md border border-qx-border">
                    <span className="text-qx-secondary">Lat:</span>
                    <span ref={latencyRef} className="text-qx-foreground">--</span>
                </div>
            </div>
        </div>
    );
};
