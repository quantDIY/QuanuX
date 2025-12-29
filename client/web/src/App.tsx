import React, { useState } from 'react';
import { LaunchAnimation } from './components/LaunchAnimation';
import { AuthPortal } from './components/AuthPortal';
import { Sidebar } from './components/Sidebar';
import { StrategyBuilder } from './pages/StrategyBuilder';
import { Integrations } from './pages/Integrations';
import { Backtests } from './pages/Backtests';

export const App = () => {
    const [stage, setStage] = useState<'launch' | 'auth' | 'dashboard'>('launch');
    const [currentView, setCurrentView] = useState(() => {
        const path = window.location.pathname;
        if (path.includes('strategy')) return 'strategy';
        if (path.includes('integrations')) return 'integrations';
        if (path.includes('backtests')) return 'backtests';
        return 'dashboard';
    });

    const handleAnimationComplete = () => {
        setStage('auth');
    };

    const handleLogin = () => {
        setStage('dashboard');
    };

    if (stage === 'launch') {
        return <LaunchAnimation onComplete={handleAnimationComplete} />;
    }

    if (stage === 'auth') {
        return <AuthPortal onLogin={handleLogin} />;
    }

    const renderContent = () => {
        switch (currentView) {
            case 'strategy':
                return <StrategyBuilder />;
            case 'integrations':
                return <Integrations />;
            case 'backtests':
                return <Backtests />;
            case 'dashboard':
            default:
                return (
                    <div className="rounded-2xl bg-card border border-border/50 p-8 text-center h-full">
                        <h2 className="text-2xl font-bold mb-4">Welcome to Your Web Portal</h2>
                        <p className="text-muted-foreground max-w-lg mx-auto mb-8">
                            Manage your server connections, view aggregated reports, and configure your QuanuX ecosystem from anywhere.
                        </p>
                        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6 text-left">
                            <div className="p-6 rounded-xl bg-background/50 border border-border hover:border-primary/50 transition-colors">
                                <h3 className="font-bold mb-2 text-primary">Server Manager</h3>
                                <p className="text-sm text-muted-foreground">0 servers connected.</p>
                            </div>
                            <div className="p-6 rounded-xl bg-background/50 border border-border hover:border-secondary/50 transition-colors">
                                <h3 className="font-bold mb-2 text-secondary">Billing & Usage</h3>
                                <p className="text-sm text-muted-foreground">Free Tier</p>
                            </div>
                            <div className="p-6 rounded-xl bg-background/50 border border-border hover:border-accent/50 transition-colors">
                                <h3 className="font-bold mb-2 text-accent">API Keys</h3>
                                <p className="text-sm text-muted-foreground">1 Active Key</p>
                            </div>
                        </div>
                    </div>
                );
        }
    };

    // Web Dashboard
    return (
        <div className="flex h-screen bg-background text-foreground overflow-hidden fade-in">
            <Sidebar currentView={currentView} onNavigate={setCurrentView} />

            <div className="flex-1 flex flex-col min-w-0 overflow-auto">
                <header className="flex justify-between items-center p-8 pb-4">
                    <h1 className="text-4xl font-black text-transparent bg-clip-text bg-gradient-to-r from-primary to-secondary">QuanuX Web</h1>
                    <div className="flex items-center gap-4">
                        <div className="w-8 h-8 rounded-full bg-muted flex items-center justify-center text-xs font-bold text-muted-foreground border border-border">DM</div>
                    </div>
                </header>

                <main className="flex-1 p-8 pt-0 h-full overflow-hidden">
                    {renderContent()}
                </main>
            </div>
        </div>
    );
};
