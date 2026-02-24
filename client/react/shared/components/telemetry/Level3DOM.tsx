import React, { useEffect, useState } from "react";

export interface L3Snapshot {
    best_bid: number;
    best_ask: number;
    alpha: number;
    tsc_lo: number;
    execution_state: number;
}

interface Level3DOMProps {
    subscribe?: (onSnapshot: (snapshot: L3Snapshot) => void) => () => void;
}

export const Level3DOM: React.FC<Level3DOMProps> = ({ subscribe }) => {
    const [snapshot, setSnapshot] = useState<L3Snapshot | null>(null);

    useEffect(() => {
        if (!subscribe) return;
        const unsubscribe = subscribe(setSnapshot);
        return () => {
            if (unsubscribe) unsubscribe();
        };
    }, [subscribe]);

    if (!snapshot) {
        return (
            <div className="bg-qx-surface border border-qx-border rounded-xl p-4 flex flex-col items-center justify-center space-y-2 h-48 shadow-lg font-mono text-xs">
                <span className="text-muted-foreground animate-pulse">Awaiting L3 Hardware Tap...</span>
            </div>
        );
    }

    const { best_bid, best_ask, alpha, tsc_lo, execution_state } = snapshot;
    const spread = (best_ask - best_bid).toFixed(2);

    // Alpha visualization mapping
    const alphaColor = alpha > 0.5 ? "text-qx-accent" : alpha < -0.5 ? "text-qx-destructive" : "text-qx-primary";

    // Chaos Protocol: Red Screen Hardware HALT mapping
    if (execution_state === 4) {
        return (
            <div className="bg-red-950/80 border border-red-500 rounded-xl p-4 flex flex-col items-center justify-center space-y-4 shadow-[0_0_30px_rgba(239,68,68,0.4)] h-full font-mono text-xs w-full animate-pulse transition-colors">
                <div className="flex justify-between items-center w-full border-b border-red-500/50 pb-2">
                    <h3 className="text-red-500 font-bold tracking-widest uppercase flex items-center gap-2">
                        <span className="w-3 h-3 rounded-full bg-red-500" />
                        L3 INTERLOCK ENGAGED
                    </h3>
                </div>
                <div className="flex flex-col items-center text-center space-y-1">
                    <span className="text-4xl text-white font-black tracking-widest">HALT</span>
                    <span className="text-red-400 font-bold tracking-widest">SENTINEL OVERRIDE</span>
                    <span className="text-red-300/80 mt-2">Hardware-enforced bitmask assertion executed. Spreader locked.</span>
                    <span className="text-red-200 mt-4 bg-red-900/50 px-2 py-1 rounded">TSC_LO: {tsc_lo}</span>
                </div>
            </div>
        );
    }

    return (
        <div className="bg-qx-surface border border-qx-border rounded-xl p-4 flex flex-col space-y-4 shadow-lg h-full font-mono text-xs">
            <div className="flex justify-between items-center border-b border-qx-border pb-2">
                <h3 className="text-qx-foreground font-bold tracking-widest uppercase flex items-center gap-2">
                    <span className="w-2 h-2 rounded-full bg-qx-primary animate-pulse" />
                    Level 3 Hardware Tap
                </h3>
                <span className="text-muted-foreground bg-background px-2 py-0.5 rounded border border-qx-border/50">
                    TSC: <span className="text-qx-secondary">{tsc_lo}</span>
                </span>
            </div>

            <div className="flex-1 grid grid-cols-2 gap-4">
                {/* Book Representation */}
                <div className="flex flex-col space-y-2 justify-center border-r border-qx-border/50 pr-4">
                    <div className="flex justify-between text-qx-destructive font-bold">
                        <span>ASK</span>
                        <span>{best_ask.toFixed(2)}</span>
                    </div>
                    <div className="flex justify-between text-muted-foreground border-y border-qx-border/30 py-1">
                        <span>SPREAD</span>
                        <span>{spread}</span>
                    </div>
                    <div className="flex justify-between text-qx-accent font-bold">
                        <span>BID</span>
                        <span>{best_bid.toFixed(2)}</span>
                    </div>
                </div>

                {/* Analytical Representation */}
                <div className="flex flex-col justify-center items-center space-y-2">
                    <span className="text-muted-foreground">ALPHA SIGNAL</span>
                    <span className={`text-4xl font-black ${alphaColor}`}>
                        {alpha > 0 ? "+" : ""}{alpha.toFixed(3)}
                    </span>
                    <span className="text-muted-foreground/60">Raw C++ Output</span>
                </div>
            </div>
        </div>
    );
};
