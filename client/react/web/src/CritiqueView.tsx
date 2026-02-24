import React, { useEffect, useState } from 'react';

// The "Two-Night Rental" Web Theater
// Orchestrated by Le Critique in The Annex

interface CritiqueViewProps {
    eventId: string;
}

export const CritiqueView: React.FC<CritiqueViewProps> = ({ eventId }) => {
    const [jaquesMode, setJaquesMode] = useState<boolean>(false);
    const [pingBack, setPingBack] = useState<string>('');
    const [auditData, setAuditData] = useState<any[]>([]);

    useEffect(() => {
        // In reality, this would query the NATS GraphQL bridge or DuckDB HTTP endpoint.
        // We simulate the NATS stream ping-back from Le Critique.

        // Simulating the detection of the `~/.quanux_jaques_mode` server state
        // The server injects a custom HTTP header or GraphQL state:
        const urlParams = new URLSearchParams(window.location.search);
        const isJaquesMode = urlParams.get('jaques') === 'true'; // Dev toggle for simulation

        setJaquesMode(isJaquesMode);

        const mockFetch = async () => {
            setAuditData([
                { tsc: "249871110023", state: "STATE_VIGIL", risk: "50", packet: "[NO_OP]" },
                { tsc: "249871110057", state: "STATE_ENGAGED", risk: "50", packet: "35=D|55=ESM4|54=1" },
            ]);

            setTimeout(() => {
                if (isJaquesMode) {
                    setPingBack("Received your little data packet. It is quite... industrial. I have filed it in my DuckDB vault. Do try to show more flair next time, Les Incompétents.");
                }
            }, 1000);
        };

        mockFetch();
    }, [eventId]);

    // The Baroque Mode Styling vs Standard Industrial
    const containerClasses = jaquesMode
        ? "flex h-screen w-screen flex-col bg-[#1A0B00] text-[#D4AF37] font-serif overflow-hidden border-[12px] border-[#8B6508] relative"
        : "flex h-screen w-screen flex-col bg-background text-foreground font-mono overflow-hidden break-words p-8";

    return (
        <div className={containerClasses}>
            {jaquesMode && (
                <>
                    <div className="absolute top-0 left-0 w-full h-8 bg-gradient-to-r from-[#8B6508] via-[#D4AF37] to-[#8B6508] opacity-50" />
                    <div className="absolute bottom-0 left-0 w-full h-8 bg-gradient-to-r from-[#8B6508] via-[#D4AF37] to-[#8B6508] opacity-50" />
                </>
            )}

            <div className={jaquesMode ? "p-12 z-10" : "z-10"}>
                <h1 className={jaquesMode ? "text-5xl border-b-2 border-[#D4AF37] pb-4 mb-8 italic text-center font-bold" : "text-2xl border-b border-primary pb-2 mb-6 uppercase"}>
                    {jaquesMode ? "Le Galerie d'Audit (The Annex)" : "QuanuX Audit Log"}
                </h1>

                <h2 className="text-xl mb-4">Event Checksum: {eventId}</h2>

                <div className={jaquesMode ? "bg-[#2A1B10] border border-[#8B6508] p-6 shadow-2xl space-y-4 mb-8" : "bg-muted p-4 space-y-2 mb-6"}>
                    {auditData.map((d, i) => (
                        <div key={i} className="flex gap-4">
                            <span className="opacity-70">[{d.tsc}]</span>
                            <span className={jaquesMode ? "text-white" : "text-primary"}>{d.state}</span>
                            <span className="opacity-50">RISK:{d.risk}</span>
                            <span>{d.packet}</span>
                        </div>
                    ))}
                </div>

                {pingBack && (
                    <div className="mt-8 p-6 bg-[#3A0F0F] border-2 border-[#D4AF37] text-xl italic text-center text-[#E5D08C] shadow-2xl animate-fade-in">
                        "{pingBack}"
                        <div className="text-right text-sm mt-4 opacity-70">- Le Critique</div>
                    </div>
                )}

                <div className={jaquesMode ? "mt-auto pt-16 text-center text-[#8B6508] uppercase text-sm tracking-[0.3em]" : "mt-auto pt-8 text-center opacity-50 text-xs uppercase"}>
                    We have a two-night rental policy... Late fee: $0.00. Please be kind, rewind.
                </div>
            </div>
        </div>
    );
};
