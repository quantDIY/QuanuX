import React, { useState } from 'react';
import { listen } from '@tauri-apps/api/event';
import { invoke } from '@tauri-apps/api/core';
import { MarketTicker, MarketTick } from '@quanux/shared-ui/components/domain/MarketTicker';
import { JitterChart } from '@quanux/shared-ui/components/telemetry/JitterChart';
import { StrategyEditor } from '@quanux/shared-ui/components/forge/StrategyEditor';
import { BuildLog } from '@quanux/shared-ui/components/forge/BuildLog';

// QuanuX Connector: Desktop (Tauri rust backend via JSON/Bincode bypass)
const tauriSubscribe = (onTick: (tick: MarketTick) => void) => {
    let unlisten: (() => void) | undefined;

    // Listen directly to the zero-copy parsed Tauri event
    listen<MarketTick>('market-tick', (event) => {
        onTick(event.payload);
    }).then(_unlisten => {
        unlisten = _unlisten;
    }).catch(console.error);

    return () => {
        if (unlisten) unlisten();
    };
};

const fireDesktopCommand = async () => {
    try {
        // Fast-path to NATS COMMAND.BIN: 1 = Mock EXECUTE, Signature = 0xBEEF
        await invoke('invoke_execution_trigger', { commandType: 1, signature: 0xBEEF });
    } catch (e) {
        console.error("Execution Trigger Failed:", e);
    }
};

export const App = () => {
    const [isDeploying, setIsDeploying] = useState(false);
    const [buildLogs, setBuildLogs] = useState<string[]>([]);
    const [currentHash, setCurrentHash] = useState<string | null>(null);

    const handleDeploy = (code: string) => {
        setIsDeploying(true);
        setBuildLogs(["> INITIALIZING QUANUX FOUNDRY..."]);
        setCurrentHash(null);

        // Simulate GraphQL Subscription streaming the Cython Build Pipeline
        setTimeout(() => setBuildLogs(l => [...l, "> Validating Python Strategy Syntax... SUCCESS"]), 500);
        setTimeout(() => setBuildLogs(l => [...l, "> Generating C++ Bindings via Cython..."]), 1000);
        setTimeout(() => setBuildLogs(l => [...l, "> Compiling aarch64 binary with -O3 optimizations..."]), 2000);
        setTimeout(() => setBuildLogs(l => [...l, "> Injecting IStrategy interfaces..."]), 3000);
        setTimeout(() => setBuildLogs(l => [...l, "> Signing SHA-256 Checksum..."]), 3500);
        setTimeout(() => {
            const hash = Array.from(crypto.getRandomValues(new Uint8Array(16)))
                .map(b => b.toString(16).padStart(2, '0')).join('');
            setBuildLogs(l => [...l, `> BINARY SIGNED: ${hash}`]);
            setTimeout(() => setBuildLogs(l => [...l, "> Sending NATS HOT_SWAP packet to Execution Node..."]), 500);
            setTimeout(() => {
                setBuildLogs(l => [...l, "> EXECUTION NODE KERNEL REPLACED. STRATEGY DEPLOYED."]);
                setCurrentHash(hash);
                setIsDeploying(false);
            }, 1500);
        }, 4000);
    };

    return (
        <div className="flex h-screen w-screen flex-col bg-background text-foreground font-mono overflow-hidden fade-in">
            <header className="p-6 border-b border-qx-border bg-qx-surface flex justify-between items-center z-10 shadow-md">
                <div className="flex items-center gap-4">
                    <h1 className="text-2xl font-black text-transparent bg-clip-text bg-gradient-to-r from-qx-primary to-qx-secondary tracking-widest uppercase">QuanuX Core</h1>
                    <span className="text-xs px-2 py-1 rounded bg-qx-border text-muted-foreground border border-qx-surface">DAEMON ACTIVE</span>
                </div>
                <div className="flex items-center gap-3">
                    <div className="w-2 h-2 rounded-full bg-qx-accent shadow-[0_0_10px_rgba(41,255,100,0.8)] animate-pulse"></div>
                    <span className="text-xs font-mono text-muted-foreground tracking-widest">NATS INGRESS</span>
                </div>
            </header>

            <main className="flex-1 p-8 grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 xl:grid-cols-4 gap-6 bg-[radial-gradient(ellipse_at_top,_var(--tw-gradient-stops))] from-qx-surface to-background">
                {/* Transpiled Figma Art - Decoupled from Mock Data */}
                <div className="col-span-1 md:col-span-1 lg:col-span-1 xl:col-span-1 space-y-6">
                    <MarketTicker symbol="BTC-PERP" subscribe={tauriSubscribe} />
                    <JitterChart
                        title="Desktop Latency (IPC)"
                        description="Tauri Rust Backend bypass -> React Ref-Buffer"
                        color="hsl(var(--color-qx-primary))"
                        subscribe={tauriSubscribe}
                        fireCommand={fireDesktopCommand}
                    />
                </div>

                <div className="col-span-1 md:col-span-1 lg:col-span-2 xl:col-span-2">
                    <StrategyEditor onDeploy={handleDeploy} isDeploying={isDeploying} />
                </div>

                <div className="col-span-1 md:col-span-2 lg:col-span-3 xl:col-span-1">
                    <BuildLog logs={buildLogs} currentHash={currentHash} />
                </div>
            </main>
        </div>
    );
};
