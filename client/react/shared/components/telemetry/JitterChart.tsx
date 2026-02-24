import React, { useEffect, useRef } from "react";
import { MarketTick } from "../domain/MarketTicker";

interface JitterChartProps {
    title: string;
    description: string;
    color: string;
    /** Hook to the raw telemetry stream */
    subscribe: (onTick: (tick: MarketTick) => void) => () => void;
    fireCommand?: () => Promise<void>;
}

export const JitterChart: React.FC<JitterChartProps> = ({ title, description, color, subscribe, fireCommand }) => {
    const canvasRef = useRef<HTMLCanvasElement>(null);
    const latestTickRef = useRef<MarketTick | null>(null);
    const pointsRef = useRef<number[]>([]);
    const MAX_POINTS = 200;

    const pendingCommandTsRef = useRef<number | null>(null);
    const lastCmdLatencyRef = useRef<number | null>(null);

    const handleFireCommand = async () => {
        if (!fireCommand) return;
        pendingCommandTsRef.current = (performance.timeOrigin + performance.now()) * 1_000_000; // in ns
        await fireCommand();
    };

    useEffect(() => {
        const unsubscribe = subscribe((tick: MarketTick) => {
            latestTickRef.current = tick;
            if (pendingCommandTsRef.current !== null) {
                const nowNs = (performance.timeOrigin + performance.now()) * 1_000_000;
                lastCmdLatencyRef.current = (nowNs - pendingCommandTsRef.current) / 1_000_000; // ms
                pendingCommandTsRef.current = null;
            }
        });

        const canvas = canvasRef.current;
        if (!canvas) return;
        const ctx = canvas.getContext("2d");
        if (!ctx) return;

        let animationFrameId: number;

        const renderLoop = () => {
            const tick = latestTickRef.current;
            if (tick) {
                // Calculate strictly the render latency natively
                const renderTsc = (performance.timeOrigin + performance.now()) * 1_000_000;
                // Total delta from C++ HFT intake to physical pixel paint
                const deltaNs = renderTsc - tick.internal_arrival_ts;
                // Convert to milliseconds for charting
                const deltaMs = deltaNs / 1_000_000;

                // Push point to the array
                pointsRef.current.push(deltaMs);
                if (pointsRef.current.length > MAX_POINTS) {
                    pointsRef.current.shift();
                }

                // Clear canvas
                ctx.clearRect(0, 0, canvas.width, canvas.height);

                // Draw standard grid lines (1ms, 2ms, etc.)
                ctx.strokeStyle = "rgba(100, 100, 100, 0.2)";
                ctx.lineWidth = 1;
                for (let i = 1; i <= 3; i++) {
                    const y = canvas.height - (i * canvas.height / 4);
                    ctx.beginPath();
                    ctx.moveTo(0, y);
                    ctx.lineTo(canvas.width, y);
                    ctx.stroke();
                }

                // Draw points
                const pts = pointsRef.current;
                if (pts.length > 0) {
                    ctx.beginPath();
                    // We assume 3ms max for the chart scale as baseline, cap at 3ms for drawing
                    const scaleY = canvas.height / 3.0;

                    for (let i = 0; i < pts.length; i++) {
                        const x = (i / MAX_POINTS) * canvas.width;
                        const y = canvas.height - Math.min(pts[i] * scaleY, canvas.height);

                        if (i === 0) {
                            ctx.moveTo(x, y);
                        } else {
                            ctx.lineTo(x, y);
                        }
                    }

                    ctx.strokeStyle = color;
                    ctx.lineWidth = 2;
                    ctx.lineJoin = "round";
                    ctx.stroke();

                    // Fill under the line
                    ctx.lineTo(canvas.width, canvas.height);
                    ctx.lineTo(0, canvas.height);
                    ctx.fillStyle = color.replace("hsl", "hsla").replace(")", ", 0.15)");
                    if (color.startsWith("rgb") || color.startsWith("#")) {
                        ctx.fillStyle = `${color}33`; // simple hex transp
                    }
                    if (color.includes("qx-primary")) ctx.fillStyle = "rgba(41, 121, 255, 0.15)";
                    if (color.includes("qx-secondary")) ctx.fillStyle = "rgba(140, 60, 255, 0.15)";

                    ctx.fill();

                    // Current Value text
                    ctx.fillStyle = "#ffffff";
                    ctx.font = "12px monospace";
                    ctx.textAlign = "right";
                    ctx.fillText(`${pts[pts.length - 1].toFixed(2)}ms`, canvas.width - 5, 15);

                    ctx.fillStyle = "rgba(255, 100, 100, 0.9)";
                    ctx.fillText(`Cmd RT: ${lastCmdLatencyRef.current ? lastCmdLatencyRef.current.toFixed(2) + 'ms' : '--'}`, canvas.width - 5, 30);
                }
            }

            animationFrameId = requestAnimationFrame(renderLoop);
        };

        animationFrameId = requestAnimationFrame(renderLoop);

        return () => {
            unsubscribe();
            cancelAnimationFrame(animationFrameId);
        };
    }, [subscribe, color]);

    return (
        <div className="bg-qx-surface border border-qx-border rounded-xl p-4 flex flex-col space-y-2 shadow-lg hover:shadow-xl transition-all">
            <div className="flex justify-between items-center">
                <div>
                    <h3 className="text-qx-foreground font-bold tracking-wide text-sm">{title}</h3>
                    <p className="text-xs text-muted-foreground">{description}</p>
                </div>
                {fireCommand && (
                    <button
                        onClick={handleFireCommand}
                        className="px-3 py-1 text-xs font-black tracking-widest bg-qx-destructive/20 text-qx-destructive border border-qx-destructive/50 rounded-md hover:bg-qx-destructive hover:text-white transition-colors uppercase"
                    >
                        Trigger
                    </button>
                )}
            </div>
            <div className="relative w-full h-32 mt-2 bg-background/50 rounded-lg overflow-hidden border border-qx-border/50">
                <canvas
                    ref={canvasRef}
                    className="absolute inset-0 w-full h-full"
                    width={400}
                    height={100}
                />
            </div>
        </div>
    );
};
