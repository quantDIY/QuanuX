import React, { useState } from 'react';
import { api } from '@quanux/shared/lib/api';

// Simple Icons
const ServerIcon = () => (
    <svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round"><rect width="20" height="8" x="2" y="2" rx="2" ry="2" /><rect width="20" height="8" x="2" y="14" rx="2" ry="2" /><line x1="6" x2="6.01" y1="6" y2="6" /><line x1="6" x2="6.01" y1="18" y2="18" /></svg>
);
const GlobeIcon = () => (
    <svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round"><circle cx="12" cy="12" r="10" /><path d="M12 2a14.5 14.5 0 0 0 0 20 14.5 14.5 0 0 0 0-20" /><path d="M2 12h20" /></svg>
);

export const ConnectServer = ({ onConnect }: { onConnect: () => void }) => {
    const [mode, setMode] = useState<'local' | 'remote'>('local');
    const [loading, setLoading] = useState(false);
    const [error, setError] = useState<string | null>(null);

    // Form State
    const [username, setUsername] = useState('');
    const [password, setPassword] = useState('');

    const handleConnect = async (e: React.FormEvent) => {
        e.preventDefault();
        setLoading(true);
        setError(null);

        if (mode === 'local') {
            // Local mode bypasses auth for now (or uses local token)
            setTimeout(() => {
                setLoading(false);
                onConnect();
            }, 1000);
            return;
        }

        try {
            // Real Remote Login
            const res = await api.login(username, password);
            console.log("Logged in:", res);
            setLoading(false);
            onConnect();
        } catch (err: any) {
            setLoading(false);
            console.error(err);
            setError("Connection failed. Please check credentials.");
        }
    };

    return (
        <div className="flex flex-col items-center justify-center min-h-screen p-4 fade-in">
            <div className="w-full max-w-md p-8 rounded-2xl bg-card border border-border/50 backdrop-blur-xl shadow-[0_0_50px_rgba(0,0,0,0.5)]">
                <div className="text-center mb-8">
                    <h2 className="text-2xl font-bold text-foreground mb-2">Connect to Server</h2>
                    <p className="text-muted-foreground text-sm">Select your QuanuX instance</p>
                </div>

                {/* Toggle */}
                <div className="flex p-1 mb-8 bg-muted/50 rounded-lg">
                    <button
                        type="button"
                        onClick={() => { setMode('local'); setError(null); }}
                        className={`flex-1 flex items-center justify-center gap-2 py-2 text-sm font-medium rounded-md transition-all ${mode === 'local' ? 'bg-primary text-primary-foreground shadow-lg shadow-primary/25' : 'text-muted-foreground hover:text-foreground'}`}
                    >
                        <ServerIcon /> Local
                    </button>
                    <button
                        type="button"
                        onClick={() => { setMode('remote'); setError(null); }}
                        className={`flex-1 flex items-center justify-center gap-2 py-2 text-sm font-medium rounded-md transition-all ${mode === 'remote' ? 'bg-secondary text-secondary-foreground shadow-lg shadow-secondary/25' : 'text-muted-foreground hover:text-foreground'}`}
                    >
                        <GlobeIcon /> Remote
                    </button>
                </div>

                <form onSubmit={handleConnect} className="space-y-4">
                    {mode === 'local' ? (
                        <div className="space-y-4">
                            <div className="p-4 rounded-lg border border-border bg-background/50 flex items-center gap-4 cursor-pointer hover:border-primary transition-colors">
                                <div className="w-10 h-10 rounded-full bg-primary/20 flex items-center justify-center text-primary">
                                    <ServerIcon />
                                </div>
                                <div>
                                    <h3 className="font-medium text-foreground">Desktop Instance</h3>
                                    <p className="text-xs text-muted-foreground">Running on localhost:8080</p>
                                </div>
                                <div className="ml-auto w-3 h-3 rounded-full bg-green-500 shadow-[0_0_10px_#00ff00]"></div>
                            </div>
                        </div>
                    ) : (
                        <div className="space-y-4">
                            <div className="space-y-2">
                                <label className="text-xs uppercase font-bold text-muted-foreground tracking-wider">Server URL</label>
                                <input type="text" placeholder="api/auth/login" disabled className="w-full bg-background/50 border border-input rounded-md px-4 py-2 text-foreground/50 cursor-not-allowed" />
                            </div>
                            <div className="space-y-2">
                                <label className="text-xs uppercase font-bold text-muted-foreground tracking-wider">Username</label>
                                <input
                                    type="text"
                                    value={username}
                                    onChange={(e) => setUsername(e.target.value)}
                                    className="w-full bg-background/50 border border-input rounded-md px-4 py-2 text-foreground focus:outline-none focus:ring-2 focus:ring-secondary/50 transition-all"
                                />
                            </div>
                            <div className="space-y-2">
                                <label className="text-xs uppercase font-bold text-muted-foreground tracking-wider">Password</label>
                                <input
                                    type="password"
                                    value={password}
                                    onChange={(e) => setPassword(e.target.value)}
                                    className="w-full bg-background/50 border border-input rounded-md px-4 py-2 text-foreground focus:outline-none focus:ring-2 focus:ring-secondary/50 transition-all"
                                />
                            </div>
                            <div className="flex items-center gap-2">
                                <input type="checkbox" id="remember" className="rounded bg-background border-input text-secondary focus:ring-secondary" />
                                <label htmlFor="remember" className="text-sm text-foreground">Remember me</label>
                            </div>
                        </div>
                    )}

                    {error && (
                        <div className="p-3 bg-destructive/10 border border-destructive/20 rounded-md text-xs text-destructive text-center">
                            {error}
                        </div>
                    )}

                    <button
                        type="submit"
                        disabled={loading}
                        className={`w-full mt-6 py-3 rounded-md font-bold text-background transition-all transform active:scale-95 ${mode === 'local' ? 'bg-primary hover:bg-primary/90 shadow-[0_0_20px_rgba(255,0,255,0.4)]' : 'bg-secondary hover:bg-secondary/90 shadow-[0_0_20px_rgba(0,255,255,0.4)]'} disabled:opacity-50 disabled:cursor-not-allowed`}
                    >
                        {loading ? 'Connecting...' : 'Connect'}
                    </button>
                </form>
            </div>
        </div>
    );
};
