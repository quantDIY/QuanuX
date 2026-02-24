import React, { useEffect, useRef } from "react";

interface BuildLogProps {
    logs: string[];
    currentHash: string | null;
}

export const BuildLog: React.FC<BuildLogProps> = ({ logs, currentHash }) => {
    const endOfLogRef = useRef<HTMLDivElement>(null);

    useEffect(() => {
        if (endOfLogRef.current) {
            endOfLogRef.current.scrollIntoView({ behavior: "smooth" });
        }
    }, [logs]);

    return (
        <div className="bg-qx-surface border border-qx-border rounded-xl p-4 flex flex-col space-y-4 shadow-lg font-mono text-xs">
            <div className="flex justify-between items-center border-b border-qx-border pb-2">
                <h3 className="text-qx-foreground font-bold tracking-widest uppercase">Foundry Telemetry</h3>
                {currentHash ? (
                    <div className="flex items-center gap-2">
                        <span className="text-muted-foreground">SHA-256:</span>
                        <span className="text-qx-accent select-all bg-qx-accent/10 px-2 py-0.5 rounded border border-qx-accent/20">
                            {currentHash}
                        </span>
                    </div>
                ) : (
                    <span className="text-muted-foreground">Awaiting compilation...</span>
                )}
            </div>

            <div className="flex-1 overflow-y-auto space-y-1 h-48 bg-background p-3 rounded-md border border-qx-border/50">
                {logs.length === 0 ? (
                    <div className="text-muted-foreground/50 italic">Waiting for Foundry uplink...</div>
                ) : (
                    logs.map((log, index) => (
                        <div key={index} className={`
                            ${log.includes("ERROR") ? "text-qx-destructive" : ""}
                            ${log.includes("SUCCESS") || log.includes("DEPLOYED") ? "text-qx-accent font-bold" : ""}
                            ${log.startsWith(">") ? "text-qx-primary" : "text-muted-foreground"}
                        `}>
                            {log}
                        </div>
                    ))
                )}
                <div ref={endOfLogRef} />
            </div>
        </div>
    );
};
