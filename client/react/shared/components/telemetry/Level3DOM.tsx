import React, { useEffect, useState } from "react";

export interface L3Snapshot {
    best_bid: number;
    best_ask: number;
    alpha: number;
    tsc_lo: number;
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

    const { best_bid, best_ask, alpha, tsc_lo } = snapshot;
    const spread = (best_ask - best_bid).toFixed(2);

    // Alpha visualization mapping
    const alphaColor = alpha > 0.5 ? "text-qx-accent" : alpha < -0.5 ? "text-qx-destructive" : "text-qx-primary";

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
