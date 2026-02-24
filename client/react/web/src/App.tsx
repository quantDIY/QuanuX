import React from 'react';
import { MarketTicker, MarketTick } from '@quanux/shared-ui/components/domain/MarketTicker';

// QuanuX Connector: Web (GraphQL Subscription via Strawberry)
const webSubscribe = (onTick: (tick: MarketTick) => void) => {
    // STUB: Real GraphQL WebSocket client integration goes here.
    // Simulating remote NATS/GraphQL relay transmission (200ms batch cycles)
    const interval = setInterval(() => {
        const mockPrice = Math.random() > 0.5 ? 95000.50 + Math.random() * 50 : 95000.50 - Math.random() * 50;
        const now = performance.timeOrigin + performance.now();
        onTick({
            local_rec_ts: now * 1_000_000,
            exchange_ts: (now - 12) * 1_000_000,
            price: mockPrice,
            size: Math.floor(Math.random() * 100),
            flags: 0,
            instrument_id: 1, // BTC-PERP
            internal_arrival_ts: now * 1_000_000,
            // Simulating relay/network latency of ~1.2ms (1,234,567 ns) across the Strawberry GraphQL relay
            processing_start_ts: (now * 1_000_000) + 1_234_567,
        });
    }, 200);

    return () => clearInterval(interval);
};

export const App = () => {
    return (
        <div className="flex h-screen w-screen flex-col bg-background text-foreground font-mono overflow-hidden fade-in">
            <header className="p-6 border-b border-qx-border bg-qx-surface flex justify-between items-center z-10 shadow-md">
                <div className="flex items-center gap-4">
                    <h1 className="text-2xl font-black text-transparent bg-clip-text bg-gradient-to-r from-[hsl(210_100%_50%)] to-[hsl(250_100%_60%)] tracking-widest uppercase">QuanuX Web</h1>
                    <span className="text-xs px-2 py-1 rounded bg-qx-border text-muted-foreground border border-qx-surface">GRAPHQL RELAY</span>
                </div>
                <div className="flex items-center gap-3">
                    <div className="w-2 h-2 rounded-full bg-qx-primary shadow-[0_0_10px_rgba(41,121,255,0.8)] animate-pulse"></div>
                    <span className="text-xs font-mono text-muted-foreground tracking-widest">WS CONNECTED</span>
                </div>
            </header>

            <main className="flex-1 p-8 grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6 bg-[radial-gradient(ellipse_at_top,_var(--tw-gradient-stops))] from-qx-surface to-background">
                {/* Transpiled Figma Art - Decoupled from Mock Data */}
                <MarketTicker symbol="BTC-PERP" subscribe={webSubscribe} />
            </main>
        </div>
    );
};
