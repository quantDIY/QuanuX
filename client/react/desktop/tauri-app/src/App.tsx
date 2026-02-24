import React from 'react';
import { listen } from '@tauri-apps/api/event';
import { invoke } from '@tauri-apps/api/core';
import { MarketTicker, MarketTick } from '@quanux/shared-ui/components/domain/MarketTicker';
import { JitterChart } from '@quanux/shared-ui/components/telemetry/JitterChart';

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

            <main className="flex-1 p-8 grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6 bg-[radial-gradient(ellipse_at_top,_var(--tw-gradient-stops))] from-qx-surface to-background">
                {/* Transpiled Figma Art - Decoupled from Mock Data */}
                <MarketTicker symbol="BTC-PERP" subscribe={tauriSubscribe} />

                <JitterChart
                    title="Desktop Latency (IPC)"
                    description="Tauri Rust Backend bypass -> React Ref-Buffer"
                    color="hsl(var(--color-qx-primary))"
                    subscribe={tauriSubscribe}
                    fireCommand={fireDesktopCommand}
                />
            </main>
        </div>
    );
};
