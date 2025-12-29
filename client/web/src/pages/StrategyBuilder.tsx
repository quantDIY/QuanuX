
import React, { useState, useEffect } from 'react';
import { Button } from '@quanux/shared/components/ui/button';
import { StrategyWizard } from '../components/StrategyWizard';
import { FileExplorer } from '../components/FileExplorer';
import { EditorTabs } from '../components/EditorTabs';
import { Terminal, Code, Play, ChevronLeft, ChevronRight, Menu, Loader2, Save } from 'lucide-react';
import { useToast } from '@quanux/shared/hooks/use-toast';

export const StrategyBuilder = () => {
    const [isSidebarOpen, setIsSidebarOpen] = useState(true);

    // File Management State
    const [files, setFiles] = useState<Record<string, string>>({});
    const [openFiles, setOpenFiles] = useState<string[]>([]);
    const [activeFile, setActiveFile] = useState<string | null>(null);

    const [messages, setMessages] = useState<string[]>([]);
    const [isGenerating, setIsGenerating] = useState(false);
    const { toast } = useToast();

    const handleStrategyComplete = async (answers: Record<string, any>) => {
        setIsGenerating(true);
        setMessages(prev => [...prev, `[*] Sending requirements to Strategy Architect...`]);

        try {
            const response = await fetch('/api/strategy/generate', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({
                    provider: 'mock',
                    model: 'gpt-4o',
                    requirements: answers,
                    api_key: 'byo-key-placeholder'
                })
            });

            if (!response.ok) {
                throw new Error(`Build failed: ${response.statusText}`);
            }

            const data = await response.json();

            if (data.files) {
                setFiles(data.files);
                const fileList = Object.keys(data.files);
                // Open main.py by default if exists, else first file
                const initialFile = fileList.find(f => f.includes('main.py')) || fileList[0];
                if (initialFile) {
                    setOpenFiles([initialFile]);
                    setActiveFile(initialFile);
                }
                setMessages(prev => [...prev, `[+] Success: Strategy generated at ${new Date().toLocaleTimeString()}`]);

                toast({
                    title: "Strategy Generated",
                    description: "Your modular strategy has been built successfully.",
                });
            } else {
                throw new Error("Invalid response format: No files returned");
            }

        } catch (error) {
            console.error(error);
            setMessages(prev => [...prev, `[!] Error: ${error instanceof Error ? error.message : 'Unknown error'}`]);
            toast({
                title: "Generation Failed",
                description: error instanceof Error ? error.message : "An error occurred",
                variant: "destructive"
            });
        } finally {
            setIsGenerating(false);
        }
    };

    const handleFileSelect = (filename: string, content: string) => {
        if (!openFiles.includes(filename)) {
            setOpenFiles(prev => [...prev, filename]);
        }
        setActiveFile(filename);
    };

    const handleTabClose = (filename: string) => {
        const newOpenFiles = openFiles.filter(f => f !== filename);
        setOpenFiles(newOpenFiles);
        if (activeFile === filename) {
            setActiveFile(newOpenFiles[newOpenFiles.length - 1] || null);
        }
    };

    // Auto-scroll terminal
    useEffect(() => {
        const terminal = document.getElementById('terminal-output');
        if (terminal) terminal.scrollTop = terminal.scrollHeight;
    }, [messages]);

    return (
        <div className="flex h-full bg-background text-foreground animate-in fade-in duration-500 overflow-hidden">
            {/* Left Panel: Strategy Wizard */}
            <div className={`
                ${isSidebarOpen ? 'w-1/3 min-w-[350px]' : 'w-0 opacity-0'} 
                border-r border-border flex flex-col bg-muted/10 backdrop-blur-sm transition-all duration-300 ease-in-out relative
            `}>
                <div className="p-4 border-b border-border flex items-center justify-between">
                    <h2 className="font-bold text-lg whitespace-nowrap">Strategy Wizard</h2>
                    <Button variant="ghost" size="icon" onClick={() => setIsSidebarOpen(false)} className="h-8 w-8">
                        <ChevronLeft className="w-4 h-4" />
                    </Button>
                </div>

                {/* Wizard Container */}
                <div className="flex-1 overflow-hidden relative">
                    {isGenerating && (
                        <div className="absolute inset-0 bg-background/80 backdrop-blur-sm z-50 flex items-center justify-center flex-col gap-4">
                            <Loader2 className="w-8 h-8 animate-spin text-primary" />
                            <p className="text-sm font-medium animate-pulse">Designing Strategy...</p>
                        </div>
                    )}
                    <StrategyWizard onComplete={handleStrategyComplete} />
                </div>
            </div>

            {/* Right Panel: Code Workspace */}
            <div className="flex-1 flex flex-col bg-background min-w-0">
                {/* Toolbar */}
                <div className="p-2 border-b border-border flex items-center justify-between bg-muted/20">
                    <div className="flex items-center gap-2 px-2">
                        {!isSidebarOpen && (
                            <Button variant="ghost" size="icon" onClick={() => setIsSidebarOpen(true)} className="h-8 w-8 mr-2">
                                <Menu className="w-4 h-4" />
                            </Button>
                        )}
                        <span className="text-sm font-medium flex items-center gap-2">
                            <Code className="w-4 h-4 text-primary" /> Strategy Workspace
                        </span>
                    </div>
                    <div className="flex items-center gap-2">
                        <Button variant="outline" size="sm" className="h-8 gap-2">
                            <Save className="w-3 h-3" />
                            Save
                        </Button>
                        <Button size="sm" className="h-8 gap-2 bg-green-600 hover:bg-green-700 text-white">
                            <Play className="w-3 h-3" />
                            Run Backtest
                        </Button>
                    </div>
                </div>

                {/* Workspace Content: Split Explorer & Editor */}
                <div className="flex-1 flex overflow-hidden">
                    {/* File Explorer Pane */}
                    <div className="w-64 border-r border-border bg-muted/5 flex flex-col">
                        {Object.keys(files).length > 0 ? (
                            <FileExplorer
                                files={files}
                                onFileSelect={handleFileSelect}
                                selectedFile={activeFile}
                            />
                        ) : (
                            <div className="p-4 text-xs text-muted-foreground text-center">
                                No strategy generated yet.
                            </div>
                        )}
                    </div>

                    {/* Editor Pane */}
                    <div className="flex-1 flex flex-col min-w-0 bg-slate-950/30">
                        <EditorTabs
                            openFiles={openFiles}
                            activeFile={activeFile}
                            onTabSelect={setActiveFile}
                            onTabClose={handleTabClose}
                        />

                        <div className="flex-1 relative font-mono text-sm overflow-auto">
                            {activeFile && files[activeFile] ? (
                                <textarea
                                    className="w-full h-full bg-transparent p-4 text-muted-foreground resize-none focus:outline-none font-mono text-sm leading-relaxed"
                                    value={files[activeFile]}
                                    onChange={(e) => {
                                        setFiles(prev => ({ ...prev, [activeFile]: e.target.value }));
                                    }}
                                    spellCheck={false}
                                />
                            ) : (
                                <div className="flex items-center justify-center h-full text-muted-foreground/40">
                                    <div className="text-center">
                                        <Code className="w-12 h-12 mx-auto mb-2 opacity-20" />
                                        <p>Select a file to view code</p>
                                    </div>
                                </div>
                            )}
                        </div>
                    </div>
                </div>

                {/* Terminal Pane */}
                <div className="h-40 border-t border-border bg-black/90 p-2 font-mono text-xs text-green-400 overflow-y-auto" id="terminal-output">
                    <div className="flex items-center gap-2 text-muted-foreground mb-2 px-2 sticky top-0 bg-black/90 pb-2 border-b border-zinc-800">
                        <Terminal className="w-3 h-3" />
                        <span>System Console</span>
                    </div>
                    <div className="px-4 space-y-1">
                        <span className="text-blue-400">USER@QUANUX</span>:<span className="text-blue-200">~</span>$ system_ready<br />
                        {messages.map((msg, i) => (
                            <div key={i} className="break-all">{msg}</div>
                        ))}
                    </div>
                </div>
            </div>
        </div>
    );
};
