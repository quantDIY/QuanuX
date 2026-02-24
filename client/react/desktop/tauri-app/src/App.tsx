import React from 'react';

export const App = () => {
    return (
        <div className="flex h-screen w-screen items-center justify-center bg-background text-foreground font-mono">
            <div className="text-center space-y-4">
                <h1 className="text-3xl font-bold tracking-widest uppercase">QuanuX</h1>
                <p className="text-sm text-muted-foreground">Tauri Desktop Shell initialized. Awaiting UI configuration.</p>
            </div>
        </div>
    );
};
