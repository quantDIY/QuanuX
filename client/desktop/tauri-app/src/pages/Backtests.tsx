import React from 'react';
import { Card } from '@quanux/shared/components/ui/card';
import { LineChart, ArrowUpRight, ArrowDownRight, Clock, MoreHorizontal } from 'lucide-react';

export const Backtests = () => {
    return (
        <div className="p-8 h-full bg-background overflow-auto animate-in fade-in duration-500">
            <div className="flex items-center justify-between mb-8">
                <div>
                    <h1 className="text-3xl font-bold bg-clip-text text-transparent bg-gradient-to-r from-primary to-secondary">Backtest Results</h1>
                    <p className="text-muted-foreground mt-2">Manage and analyze your strategy performance.</p>
                </div>
            </div>

            <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6">
                {[1, 2, 3].map((i) => (
                    <Card key={i} className="p-6 hover:border-primary/50 transition-all cursor-pointer group">
                        <div className="flex items-center justify-between mb-4">
                            <div className="flex items-center gap-2">
                                <div className="p-2 bg-primary/10 rounded-lg text-primary">
                                    <LineChart className="w-5 h-5" />
                                </div>
                                <span className="font-bold">Macd_Cross_BTC_{i}</span>
                            </div>
                            <MoreHorizontal className="w-4 h-4 text-muted-foreground" />
                        </div>

                        <div className="space-y-4">
                            <div className="flex items-center justify-between">
                                <span className="text-sm text-muted-foreground">Total Return</span>
                                <span className="text-sm font-bold text-green-500 flex items-center gap-1">
                                    +124.5% <ArrowUpRight className="w-3 h-3" />
                                </span>
                            </div>
                            <div className="flex items-center justify-between">
                                <span className="text-sm text-muted-foreground">Sharpe Ratio</span>
                                <span className="text-sm font-medium">1.85</span>
                            </div>
                            <div className="flex items-center justify-between">
                                <span className="text-sm text-muted-foreground">Max Drawdown</span>
                                <span className="text-sm font-medium text-red-400">-15.2%</span>
                            </div>
                        </div>

                        <div className="mt-4 pt-4 border-t border-border flex items-center gap-2 text-xs text-muted-foreground">
                            <Clock className="w-3 h-3" />
                            <span>Run 2 hours ago</span>
                        </div>
                    </Card>
                ))}
            </div>
        </div>
    );
};
