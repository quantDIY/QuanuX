import React, { useState } from "react";

interface StrategyEditorProps {
    onDeploy: (code: string) => void;
    isDeploying: boolean;
}

export const StrategyEditor: React.FC<StrategyEditorProps> = ({ onDeploy, isDeploying }) => {
    const [code, setCode] = useState<string>(
        `def on_tick(self, tick: MarketTick):\n    # Tactical Spread Logic\n    if tick.price > self.upper_band:\n        self.execute_short(tick)\n    elif tick.price < self.lower_band:\n        self.execute_long(tick)`
    );

    return (
        <div className="bg-qx-surface border border-qx-border rounded-xl p-4 flex flex-col space-y-4 shadow-lg h-full">
            <div className="flex justify-between items-center">
                <h3 className="text-qx-foreground font-bold tracking-wide text-sm">Strategy Forge (Python)</h3>
                <span className="text-xs px-2 py-1 bg-qx-accent/20 text-qx-accent rounded-sm border border-qx-accent/30 font-mono">
                    target: aarch64-cython
                </span>
            </div>
            <div className="flex-1 relative border border-qx-border/50 rounded-md overflow-hidden bg-background">
                <textarea
                    value={code}
                    onChange={(e) => setCode(e.target.value)}
                    className="absolute inset-0 w-full h-full bg-transparent text-qx-secondary p-4 font-mono text-sm resize-none focus:outline-none focus:ring-1 focus:ring-qx-primary"
                    spellCheck="false"
                />
            </div>
            <div className="flex justify-end pt-2">
                <button
                    onClick={() => onDeploy(code)}
                    disabled={isDeploying}
                    className={`px-6 py-2 rounded-md font-bold tracking-widest text-sm uppercase transition-all shadow-md ${isDeploying
                            ? "bg-qx-border text-muted-foreground cursor-not-allowed"
                            : "bg-qx-primary text-background hover:bg-white hover:shadow-[0_0_15px_rgba(41,121,255,0.6)]"
                        }`}
                >
                    {isDeploying ? "Compiling..." : "Deploy to C++"}
                </button>
            </div>
        </div>
    );
};
