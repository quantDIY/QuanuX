import React, { useState } from 'react';
import { Button } from '@quanux/shared/components/ui/button';
import { Card } from '@quanux/shared/components/ui/card';
import { Separator } from '@quanux/shared/components/ui/separator';
import { Bot, Play, Terminal, Code, Send, Cpu, ChevronRight } from 'lucide-react';

export const StrategyBuilder = () => {
    const [input, setInput] = useState('');
    const [messages, setMessages] = useState<{ role: 'user' | 'ai', content: string }[]>([
        { role: 'ai', content: "Hello! I'm your Quant Assistant. I can help you build a trading strategy using vectorbt, backtrader, or other frameworks. What kind of strategy would you like to build today?" }
    ]);

    const handleSend = () => {
        if (!input.trim()) return;
        setMessages([...messages, { role: 'user', content: input }]);
        setInput('');
        // Mock AI response for now
        setTimeout(() => {
            setMessages(prev => [...prev, { role: 'ai', content: "That sounds interesting. Could you specify the timeframe and the asset class you are interested in?" }]);
        }, 1000);
    };

    return (
        <div className="flex h-full bg-background text-foreground animate-in fade-in duration-500">
            {/* Left Panel: Strategy Wizard / Chat */}
            <div className="w-1/3 border-r border-border flex flex-col bg-muted/10 backdrop-blur-sm">
                <div className="p-4 border-b border-border flex items-center justify-between">
                    <div className="flex items-center gap-2">
                        <Bot className="w-5 h-5 text-primary" />
                        <h2 className="font-bold text-lg">Strategy Wizard</h2>
                    </div>
                    <div className="flex items-center gap-2 text-xs text-muted-foreground">
                        <Cpu className="w-3 h-3" />
                        <span>Model: GPT-4o</span>
                    </div>
                </div>

                <div className="flex-1 overflow-y-auto p-4 space-y-4">
                    {messages.map((msg, idx) => (
                        <div key={idx} className={`flex ${msg.role === 'user' ? 'justify-end' : 'justify-start'}`}>
                            <div className={`max-w-[85%] p-3 rounded-2xl text-sm ${msg.role === 'user'
                                ? 'bg-primary text-primary-foreground rounded-tr-none'
                                : 'bg-card border border-border rounded-tl-none shadow-sm'
                                }`}>
                                {msg.content}
                            </div>
                        </div>
                    ))}
                </div>

                <div className="p-4 border-t border-border bg-background/50">
                    <div className="relative">
                        <textarea
                            value={input}
                            onChange={(e) => setInput(e.target.value)}
                            onKeyDown={(e) => e.key === 'Enter' && !e.shiftKey && (e.preventDefault(), handleSend())}
                            placeholder="Describe your strategy..."
                            className="w-full bg-muted/50 border border-border rounded-xl p-3 pr-12 resize-none focus:outline-none focus:ring-1 focus:ring-primary h-20 text-sm"
                        />
                        <Button
                            size="icon"
                            className="absolute right-2 bottom-2 h-8 w-8"
                            onClick={handleSend}
                        >
                            <Send className="w-4 h-4" />
                        </Button>
                    </div>
                </div>
            </div>

            {/* Right Panel: Code & Results */}
            <div className="flex-1 flex flex-col bg-background">
                {/* Toolbar */}
                <div className="p-2 border-b border-border flex items-center justify-between bg-muted/20">
                    <div className="flex items-center gap-2 px-2">
                        <Code className="w-4 h-4 text-muted-foreground" />
                        <span className="text-sm font-medium">strategy.py</span>
                        <span className="text-xs text-muted-foreground ml-2">(Python 3.11)</span>
                    </div>
                    <div className="flex items-center gap-2">
                        <Button variant="outline" size="sm" className="h-8 gap-2">
                            <Terminal className="w-3 h-3" />
                            Console
                        </Button>
                        <Button size="sm" className="h-8 gap-2 bg-green-600 hover:bg-green-700 text-white">
                            <Play className="w-3 h-3" />
                            Run Backtest
                        </Button>
                    </div>
                </div>

                {/* Editor Area (Placeholder) */}
                <div className="flex-1 p-0 relative font-mono text-sm group">
                    <div className="absolute inset-0 p-4 overflow-auto text-muted-foreground bg-slate-950/50">
                        <span className="text-primary">import</span> vectorbt <span className="text-primary">as</span> vbt<br />
                        <span className="text-primary">import</span> numpy <span className="text-primary">as</span> np<br />
                        <br />
                        <span className="text-muted-foreground/60"># Strategy code will be generated here...</span><br />
                        <br />
                        price = vbt.YFData.download(<span className="text-yellow-500">'BTC-USD'</span>).get(<span className="text-yellow-500">'Close'</span>)<br />
                        fast_ma = vbt.MA.run(price, <span className="text-orange-400">10</span>)<br />
                        slow_ma = vbt.MA.run(price, <span className="text-orange-400">50</span>)<br />
                        entries = fast_ma.ma_crossed_above(slow_ma)<br />
                        exits = fast_ma.ma_crossed_below(slow_ma)<br />
                        <br />
                        pf = vbt.Portfolio.from_signals(price, entries, exits)<br />
                        print(pf.total_return())
                    </div>
                </div>

                {/* Integrated Terminal (Collapsed State) */}
                <div className="h-32 border-t border-border bg-black/90 p-2 font-mono text-xs text-green-400 overflow-y-auto">
                    <div className="flex items-center gap-2 text-muted-foreground mb-2 px-2">
                        <ChevronRight className="w-3 h-3" />
                        <span>Terminal</span>
                    </div>
                    <div className="px-4">
                        <span className="text-blue-400">USER@QUANUX</span>:<span className="text-blue-200">~</span>$ python strategy.py<br />
                        [+] Market Data Connection: OK<br />
                        [+] Strategy Loaded: moving_average_crossover<br />
                        [+] Backtest Complete. Total Return: 124.5%<br />
                    </div>
                </div>

            </div>
        </div>
    );
};
