
import React, { useEffect, useRef, useState } from 'react';
import { Terminal } from '@xterm/xterm';
import { FitAddon } from '@xterm/addon-fit';
import { WebLinksAddon } from '@xterm/addon-web-links';
import '@xterm/xterm/css/xterm.css';

interface XTermProps {
    className?: string;
    wsUrl?: string; // WebSocket URL to connect to backend PTY
}

export const XTerm: React.FC<XTermProps> = ({ className, wsUrl = "ws://localhost:8000/api/terminal/ws" }) => {
    const terminalRef = useRef<HTMLDivElement>(null);
    const xtermRef = useRef<Terminal | null>(null);
    const fitAddonRef = useRef<FitAddon | null>(null);
    const socketRef = useRef<WebSocket | null>(null);
    const [isConnected, setIsConnected] = useState(false);

    useEffect(() => {
        if (!terminalRef.current) return;

        // 1. Initialize Terminal
        const term = new Terminal({
            cursorBlink: true,
            fontSize: 14,
            fontFamily: '"Fira Code", monospace',
            theme: {
                background: '#09090b', // zinc-950
                foreground: '#a1a1aa', // zinc-400
                cursor: '#22c55e',     // green-500
                cursorAccent: '#000000',
                selectionBackground: 'rgba(34, 197, 94, 0.3)', // Green transparent
                black: '#000000',
                red: '#ef4444',
                green: '#22c55e',
                yellow: '#eab308',
                blue: '#3b82f6',
                magenta: '#d946ef',
                cyan: '#06b6d4',
                white: '#ffffff',
                brightBlack: '#71717a',
                brightRed: '#f87171',
                brightGreen: '#4ade80',
                brightYellow: '#fde047',
                brightBlue: '#60a5fa',
                brightMagenta: '#e879f9',
                brightCyan: '#22d3ee',
                brightWhite: '#fafafa',
            },
            allowProposedApi: true
        });

        const fitAddon = new FitAddon();
        const webLinksAddon = new WebLinksAddon();

        term.loadAddon(fitAddon);
        term.loadAddon(webLinksAddon);

        term.open(terminalRef.current);
        fitAddon.fit();

        xtermRef.current = term;
        fitAddonRef.current = fitAddon;

        term.writeln('\x1b[1;32mWelcome to QuanuX Terminal v1.0\x1b[0m');
        term.writeln('Connecting to local environment...\r\n');

        // 2. Connect WebSocket
        const ws = new WebSocket(wsUrl);

        ws.onopen = () => {
            setIsConnected(true);
            term.writeln('\x1b[1;34m[Connected]\x1b[0m\r\n');
            // Initial resize to sync PTY size
            fitAddon.fit();
        };

        ws.onmessage = (event) => {
            if (typeof event.data === 'string') {
                term.write(event.data);
            } else {
                // Blob handling if needed
                const reader = new FileReader();
                reader.onload = () => {
                    term.write(reader.result as string);
                };
                reader.readAsText(event.data);
            }
        };

        ws.onclose = () => {
            setIsConnected(false);
            term.writeln('\r\n\x1b[1;31m[Disconnected]\x1b[0m');
        };

        ws.onerror = (err) => {
            console.error("Terminal WS Error:", err);
            term.writeln('\r\n\x1b[1;31m[Connection Error]\x1b[0m');
        };

        socketRef.current = ws;

        // 3. Handle Input
        term.onData((data) => {
            if (ws.readyState === WebSocket.OPEN) {
                ws.send(JSON.stringify({ type: 'input', data }));
            }
        });

        // Handle Resize
        const handleResize = () => {
            fitAddon.fit();
            if (ws.readyState === WebSocket.OPEN) {
                const dims = { cols: term.cols, rows: term.rows };
                ws.send(JSON.stringify({ type: 'resize', ...dims }));
            }
        };

        window.addEventListener('resize', handleResize);
        // Also observe container size changes (e.g. sidebar toggle)
        const resizeObserver = new ResizeObserver(() => {
            handleResize();
        });
        resizeObserver.observe(terminalRef.current!);


        return () => {
            window.removeEventListener('resize', handleResize);
            resizeObserver.disconnect();
            ws.close();
            term.dispose();
        };

    }, [wsUrl]);

    return (
        <div className={`w-full h-full bg-black/90 p-1 ${className}`}>
            <div ref={terminalRef} className="w-full h-full overflow-hidden" />
        </div>
    );
};
