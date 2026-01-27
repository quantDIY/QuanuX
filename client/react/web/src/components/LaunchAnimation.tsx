import React, { useEffect, useState } from 'react';

export const LaunchAnimation = ({ onComplete }: { onComplete: () => void }) => {
    const [isExiting, setIsExiting] = useState(false);

    useEffect(() => {
        // Start exit sequence after 2.5s
        const timer = setTimeout(() => {
            setIsExiting(true);
            // Calls onComplete after transition finishes (500ms)
            setTimeout(onComplete, 500);
        }, 2500);
        return () => clearTimeout(timer);
    }, [onComplete]);

    return (
        <div className={`fixed inset-0 z-50 flex items-center justify-center bg-background transition-opacity duration-500 ${isExiting ? 'opacity-0 pointer-events-none' : 'opacity-100'}`}>
            <div className="relative">
                {/* Glowing orb background */}
                <div className="absolute -inset-8 rounded-full bg-gradient-to-r from-primary to-secondary opacity-40 blur-3xl animate-pulse"></div>

                <div className="relative flex flex-col items-center justify-center">
                    {/* Glitchy Text Effect Wrapper */}
                    <div className="relative group">
                        <h1 className="text-7xl font-black tracking-tighter text-transparent bg-clip-text bg-gradient-to-r from-primary via-purple-500 to-secondary drop-shadow-[0_0_10px_rgba(255,0,255,0.5)]">
                            QuanuX
                        </h1>
                    </div>

                    <div className="mt-6 flex flex-col items-center gap-2">
                        <div className="h-1 w-32 bg-muted rounded-full overflow-hidden">
                            <div className="h-full bg-accent animate-[loading_2s_ease-in-out_infinite] w-full origin-left"></div>
                        </div>
                        <p className="text-xs text-muted-foreground font-mono tracking-[0.2em] uppercase animate-pulse">
                            System Initialization
                        </p>
                    </div>
                </div>
            </div>
        </div>
    );
};
