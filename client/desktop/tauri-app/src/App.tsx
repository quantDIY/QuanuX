import React, { useState } from 'react';
import { LaunchAnimation } from './components/LaunchAnimation';
import { ConnectServer } from './components/ConnectServer';
import { Sidebar } from './components/Sidebar';
import { StrategyBuilder } from './pages/StrategyBuilder';
import { Integrations } from './pages/Integrations';
import { Backtests } from './pages/Backtests';
import { Settings } from './pages/Settings';

export const App = () => {
    const [stage, setStage] = useState<'launch' | 'connect' | 'dashboard'>('launch');
    const [currentView, setCurrentView] = useState(() => {
        const path = window.location.pathname;
        if (path.includes('strategy')) return 'strategy';
        if (path.includes('integrations')) return 'integrations';
        if (path.includes('backtests')) return 'backtests';
        return 'dashboard';
    });

    const handleAnimationComplete = () => {
        setStage('connect');
    };

    const handleConnect = () => {
        setStage('dashboard');
    };

    if (stage === 'launch') {
        return <LaunchAnimation onComplete={handleAnimationComplete} />;
    }

    if (stage === 'connect') {
        return <ConnectServer onConnect={handleConnect} />;
    }

    const renderContent = () => {
        switch (currentView) {
            case 'strategy':
                return <StrategyBuilder />;
            case 'integrations':
                return <Integrations />;
            case 'backtests':
                return <Backtests />;
            case 'settings':
                return <Settings />;
            case 'dashboard':
            default:
                return (
                    <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6">
                        {/* Placeholder cards */}
                        <div className="p-6 rounded-xl bg-card border border-border/50 hover:border-primary/50 transition-all duration-300 hover:shadow-[0_0_30px_rgba(255,0,255,0.1)] group cursor-pointer">
                            <h3 className="text-xl font-bold mb-2 group-hover:text-primary transition-colors">Market Data</h3>
                            <p className="text-muted-foreground tezt-sm">Live stream active. Latency: 12ms.</p>
                            <div className="mt-4 h-32 bg-background/50 rounded-lg flex items-center justify-center border border-dashed border-muted">
                                <span className="text-xs text-muted-foreground">CHART PLACEHOLDER</span>
                            </div>
                        </div>

                        <div className="p-6 rounded-xl bg-card border border-border/50 hover:border-secondary/50 transition-all duration-300 hover:shadow-[0_0_30px_rgba(0,255,255,0.1)] group cursor-pointer">
                            <h3 className="text-xl font-bold mb-2 group-hover:text-secondary transition-colors">Strategies</h3>
                            <p className="text-muted-foreground text-sm">3 Active strategies. PnL +2.4%</p>
                            <div className="mt-4 flex gap-2">
                                <div className="px-3 py-1 rounded-full bg-secondary/10 text-secondary text-xs border border-secondary/20">Alpha</div>
                                <div className="px-3 py-1 rounded-full bg-primary/10 text-primary text-xs border border-primary/20">Gamma</div>
                            </div>
                        </div>

                        <div className="p-6 rounded-xl bg-card border border-border/50 hover:border-accent/50 transition-all duration-300 hover:shadow-[0_0_30px_rgba(57,255,20,0.1)] group cursor-pointer">
                            <h3 className="text-xl font-bold mb-2 group-hover:text-accent transition-colors">System Health</h3>
                            <p className="text-muted-foreground text-sm">All services operational.</p>
                            <div className="mt-4 space-y-2">
                                <div className="flex justify-between text-xs"><span>CPU</span><span className="text-accent">12%</span></div>
                                <div className="w-full h-1 bg-muted rounded-full overflow-hidden"><div className="w-[12%] h-full bg-accent"></div></div>
                                <div className="flex justify-between text-xs"><span>MEM</span><span className="text-accent">45%</span></div>
                                <div className="w-full h-1 bg-muted rounded-full overflow-hidden"><div className="w-[45%] h-full bg-accent"></div></div>
                            </div>
                        </div>
                    </div>
                );
        }
    };

    // Dashboard / Logged In State
    return (
        <div className="flex h-screen bg-background text-foreground overflow-hidden fade-in">
            <Sidebar currentView={currentView} onNavigate={setCurrentView} />

            <div className="flex-1 flex flex-col min-w-0 overflow-auto">
                <header className="flex justify-between items-center p-8 pb-4">
                    <h1 className="text-4xl font-black text-transparent bg-clip-text bg-gradient-to-r from-primary to-secondary drop-shadow-[0_0_15px_rgba(255,0,255,0.3)]">
                        QuanuX
                    </h1>
                    <div className="flex items-center gap-4">
                        <div className="w-3 h-3 rounded-full bg-green-500 shadow-[0_0_10px_#00ff00] animate-pulse"></div>
                        <span className="text-sm font-mono text-muted-foreground">SYSTEM ONLINE</span>
                    </div>
                </header>

                <main className="flex-1 p-8 pt-0 h-full overflow-hidden">
                    {renderContent()}
                </main>
            </div>
        </div>
    );
};
