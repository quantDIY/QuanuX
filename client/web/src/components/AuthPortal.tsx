import React, { useState } from 'react';
import { api } from '@quanux/shared/lib/api';

export const AuthPortal = ({ onLogin }: { onLogin: () => void }) => {
    const [mode, setMode] = useState<'login' | 'register'>('login');
    const [loading, setLoading] = useState(false);
    const [error, setError] = useState<string | null>(null);

    // Form State
    const [clientName, setClientName] = useState('');
    const [email, setEmail] = useState('');
    const [username, setUsername] = useState('');
    const [password, setPassword] = useState('');
    const [clientId, setClientId] = useState('');
    const [clientSecret, setClientSecret] = useState('');

    const handleLogin = async (e: React.FormEvent) => {
        e.preventDefault();
        setLoading(true);
        setError(null);
        try {
            const res = await api.login(clientId || username, clientSecret || password);
            console.log("Logged in:", res);
            setLoading(false);
            onLogin();
        } catch (err: any) {
            console.error(err);
            setError("Login failed. Check credentials.");
            setLoading(false);
        }
    };

    const handleRegister = async (e: React.FormEvent) => {
        e.preventDefault();
        setLoading(true);
        setError(null);
        try {
            const res = await api.registerClient(clientName, email);
            setClientId(res.client_id);
            setClientSecret(res.client_secret);
            setMode('login'); // Switch to login to show keys or auto-fill
            // In a real app, we might auto-login or show the keys for the user to copy
            alert(`Client Registered!\nID: ${res.client_id}\nSecret: ${res.client_secret}`);
            setLoading(false);
        } catch (err: any) {
            console.error(err);
            setError("Registration failed.");
            setLoading(false);
        }
    };

    return (
        <div className="flex items-center justify-center min-h-screen p-4 fade-in">
            <div className="w-full max-w-md p-8 rounded-2xl bg-card border border-border/50 backdrop-blur-xl shadow-[0_0_50px_rgba(0,0,0,0.5)]">

                <div className="flex justify-center mb-6">
                    <h1 className="text-3xl font-black text-transparent bg-clip-text bg-gradient-to-r from-primary to-secondary">QuanuX Web</h1>
                </div>

                <div className="flex border-b border-border/50 mb-6">
                    <button
                        onClick={() => { setMode('login'); setError(null); }}
                        className={`flex-1 py-3 text-sm font-bold uppercase tracking-wider border-b-2 transition-colors ${mode === 'login' ? 'border-primary text-primary' : 'border-transparent text-muted-foreground hover:text-foreground'}`}
                    >
                        Login
                    </button>
                    <button
                        onClick={() => { setMode('register'); setError(null); }}
                        className={`flex-1 py-3 text-sm font-bold uppercase tracking-wider border-b-2 transition-colors ${mode === 'register' ? 'border-secondary text-secondary' : 'border-transparent text-muted-foreground hover:text-foreground'}`}
                    >
                        Register Client
                    </button>
                </div>

                {error && (
                    <div className="mb-4 p-3 bg-destructive/10 border border-destructive/20 rounded-md text-xs text-destructive text-center">
                        {error}
                    </div>
                )}

                <form onSubmit={mode === 'login' ? handleLogin : handleRegister} className="space-y-4">
                    {mode === 'login' ? (
                        <>
                            <div className="space-y-2">
                                <label className="text-xs uppercase font-bold text-muted-foreground tracking-wider">Client ID / Username</label>
                                <input
                                    type="text"
                                    value={clientId}
                                    onChange={(e) => setClientId(e.target.value)}
                                    className="w-full bg-background/50 border border-input rounded-md px-4 py-2 text-foreground focus:outline-none focus:ring-2 focus:ring-primary/50 transition-all text-sm font-mono"
                                />
                            </div>
                            <div className="space-y-2">
                                <label className="text-xs uppercase font-bold text-muted-foreground tracking-wider">Client Secret / Password</label>
                                <input
                                    type="password"
                                    value={clientSecret}
                                    onChange={(e) => setClientSecret(e.target.value)}
                                    className="w-full bg-background/50 border border-input rounded-md px-4 py-2 text-foreground focus:outline-none focus:ring-2 focus:ring-primary/50 transition-all text-sm font-mono"
                                />
                            </div>
                            <div className="flex items-center justify-between mt-2">
                                <label className="flex items-center gap-2 text-sm text-foreground">
                                    <input type="checkbox" className="rounded bg-background border-input text-primary focus:ring-primary" />
                                    Remember me
                                </label>
                                <a href="#" className="text-xs text-primary hover:underline">Forgot credentials?</a>
                            </div>

                            <button
                                type="submit"
                                disabled={loading}
                                className="w-full mt-6 py-3 rounded-md font-bold text-background bg-primary hover:bg-primary/90 shadow-[0_0_20px_rgba(255,0,255,0.4)] transition-all transform active:scale-95 disabled:opacity-50"
                            >
                                {loading ? 'Authenticating...' : 'Access Portal'}
                            </button>
                        </>
                    ) : (
                        <>
                            <p className="text-sm text-muted-foreground mb-4">Create a new secure client identity to access your QuanuX resources.</p>
                            <div className="space-y-2">
                                <label className="text-xs uppercase font-bold text-muted-foreground tracking-wider">Client Name</label>
                                <input
                                    type="text"
                                    placeholder="My Web Dashboard"
                                    value={clientName}
                                    onChange={(e) => setClientName(e.target.value)}
                                    className="w-full bg-background/50 border border-input rounded-md px-4 py-2 text-foreground focus:outline-none focus:ring-2 focus:ring-secondary/50 transition-all"
                                />
                            </div>
                            <div className="space-y-2">
                                <label className="text-xs uppercase font-bold text-muted-foreground tracking-wider">Email (Optional)</label>
                                <input
                                    type="email"
                                    value={email}
                                    onChange={(e) => setEmail(e.target.value)}
                                    className="w-full bg-background/50 border border-input rounded-md px-4 py-2 text-foreground focus:outline-none focus:ring-2 focus:ring-secondary/50 transition-all"
                                />
                            </div>

                            <button
                                type="submit"
                                disabled={loading}
                                className="w-full mt-6 py-3 rounded-md font-bold text-foreground bg-secondary hover:bg-secondary/90 shadow-[0_0_20px_rgba(0,255,255,0.4)] transition-all transform active:scale-95 disabled:opacity-50"
                            >
                                {loading ? 'GeneratingKeys...' : 'Create Credentials'}
                            </button>
                        </>
                    )}
                </form>
            </div>
        </div>
    );
};
