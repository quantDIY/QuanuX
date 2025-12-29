import React, { useState } from 'react';
import { Button } from '@quanux/shared/components/ui/button';
import { Input } from '@quanux/shared/components/ui/input';
import { Label } from '@quanux/shared/components/ui/label';
import { Textarea } from '@quanux/shared/components/ui/textarea';
import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue } from '@quanux/shared/components/ui/select';
import { RadioGroup, RadioGroupItem } from '@quanux/shared/components/ui/radio-group';
import { Checkbox } from '@quanux/shared/components/ui/checkbox';
import { Card, CardContent, CardFooter, CardHeader, CardTitle } from '@quanux/shared/components/ui/card';
import { ScrollArea } from '@quanux/shared/components/ui/scroll-area';
import { ChevronRight, ChevronLeft, Check, Plus, Trash2 } from 'lucide-react';

interface WizardProps {
    onComplete: (answers: Record<string, any>) => void;
}

export const StrategyWizard: React.FC<WizardProps> = ({ onComplete }) => {
    const [step, setStep] = useState(1);
    const [answers, setAnswers] = useState<Record<string, any>>({
        customQuestions: []
    });

    const updateAnswer = (key: string, value: any) => {
        setAnswers(prev => ({ ...prev, [key]: value }));
    };

    const nextStep = () => setStep(prev => prev + 1);
    const prevStep = () => setStep(prev => Math.max(1, prev - 1));

    const steps = [
        { id: 1, title: "Strategy Type", key: "type" },
        { id: 2, title: "Contracts", key: "contracts" },
        { id: 3, title: "Direction", key: "direction" },
        { id: 4, title: "Broker", key: "broker" },
        { id: 5, title: "Historical Data", key: "historical_data" },
        { id: 6, title: "Live Data", key: "live_data" },
        { id: 7, title: "Multi-Asset / Timeframe", key: "multi_asset" },
        { id: 8, title: "Indicators", key: "indicators" },
        { id: 9, title: "Indicator Settings", key: "indicator_settings" },
        { id: 10, title: "Data Pre-load", key: "data_preload" },
        { id: 11, title: "Schedule", key: "schedule" },
        { id: 12, title: "Signal Logic", key: "signal_logic" },
        { id: 13, title: "Components (Entry/Exit/Risk)", key: "components" },
        { id: 14, title: "Position Sizing", key: "sizing" },
        { id: 15, title: "Backtest Configuration", key: "backtest" },
        { id: 16, title: "Naming", key: "naming" },
        { id: 17, title: "Custom Questions", key: "custom_questions" },
    ];

    const currentStepConfig = steps.find(s => s.id === step);

    const renderStepContent = () => {
        switch (step) {
            case 1:
                return (
                    <div className="space-y-4">
                        <Label>What type of strategy are we building?</Label>
                        <Select onValueChange={(v) => updateAnswer('type', v)} defaultValue={answers.type}>
                            <SelectTrigger><SelectValue placeholder="Select type" /></SelectTrigger>
                            <SelectContent>
                                {["Entry", "Exit", "Stop", "Trailing Stop", "Full Strategy", "Signal-based", "Position Sizing", "Other"].map(opt => (
                                    <SelectItem key={opt} value={opt}>{opt}</SelectItem>
                                ))}
                            </SelectContent>
                        </Select>
                    </div>
                );
            case 2:
                return (
                    <div className="space-y-4">
                        <Label>Which contracts is this intended for? (e.g. ES, NQ, SPY)</Label>
                        <Input
                            placeholder="All / General or specific symbols..."
                            value={answers.contracts || ''}
                            onChange={e => updateAnswer('contracts', e.target.value)}
                        />
                    </div>
                );
            case 3:
                return (
                    <div className="space-y-4">
                        <Label>Is the strategy for LONG, SHORT, or BOTH?</Label>
                        <RadioGroup onValueChange={(v) => updateAnswer('direction', v)} defaultValue={answers.direction}>
                            <div className="flex items-center space-x-2"><RadioGroupItem value="LONG" id="r1" /><Label htmlFor="r1">Long Only</Label></div>
                            <div className="flex items-center space-x-2"><RadioGroupItem value="SHORT" id="r2" /><Label htmlFor="r2">Short Only</Label></div>
                            <div className="flex items-center space-x-2"><RadioGroupItem value="BOTH" id="r3" /><Label htmlFor="r3">Both</Label></div>
                        </RadioGroup>
                        {answers.direction === 'BOTH' && (
                            <p className="text-sm text-muted-foreground mt-2">
                                Note: We will configure Long logic first, allowing you to mirror or customize Short logic later.
                            </p>
                        )}
                    </div>
                );
            case 4:
                return (
                    <div className="space-y-4">
                        <Label>Which broker is this being built for?</Label>
                        <Select onValueChange={(v) => updateAnswer('broker', v)} defaultValue={answers.broker}>
                            <SelectTrigger><SelectValue placeholder="Select Broker" /></SelectTrigger>
                            <SelectContent>
                                <SelectItem value="Topstep">Topstep</SelectItem>
                                <SelectItem value="InteractiveBrokers">Interactive Brokers (IBKR)</SelectItem>
                                <SelectItem value="Tradovate">Tradovate</SelectItem>
                                <SelectItem value="Paper">Paper / Simulated</SelectItem>
                            </SelectContent>
                        </Select>
                    </div>
                );
            case 5:
                return (
                    <div className="space-y-4">
                        <Label>Historical Data Source</Label>
                        <Select onValueChange={(v) => updateAnswer('historical_data', v)} defaultValue={answers.historical_data}>
                            <SelectTrigger><SelectValue placeholder="Select Source" /></SelectTrigger>
                            <SelectContent>
                                <SelectItem value="API">Broker API</SelectItem>
                                <SelectItem value="CSV">CSV File</SelectItem>
                                <SelectItem value="Database">Local Database</SelectItem>
                            </SelectContent>
                        </Select>
                    </div>
                );
            case 6:
                return (
                    <div className="space-y-4">
                        <Label>Live Data Source</Label>
                        <Input
                            placeholder="e.g. Rithmic, IBKR, Polygon..."
                            value={answers.live_data || ''}
                            onChange={e => updateAnswer('live_data', e.target.value)}
                        />
                    </div>
                );
            case 7:
                return (
                    <div className="space-y-4">
                        <Label>Does this strategy utilize multiple contracts or timeframes?</Label>
                        <RadioGroup onValueChange={(v) => updateAnswer('multi_asset', v)} defaultValue={answers.multi_asset}>
                            <div className="flex items-center space-x-2"><RadioGroupItem value="NO" id="ma1" /><Label htmlFor="ma1">No, Single Asset/Timeframe</Label></div>
                            <div className="flex items-center space-x-2"><RadioGroupItem value="YES" id="ma2" /><Label htmlFor="ma2">Yes, Multiple</Label></div>
                        </RadioGroup>
                        {answers.multi_asset === 'YES' && (
                            <Textarea
                                placeholder="Describe the additional contracts/timeframes..."
                                value={answers.multi_asset_details || ''}
                                onChange={e => updateAnswer('multi_asset_details', e.target.value)}
                            />
                        )}
                    </div>
                );
            case 8:
                return (
                    <div className="space-y-4">
                        <Label>Which indicators does this strategy utilize?</Label>
                        <Textarea
                            placeholder="e.g. RSI (14), EMA (50), MACD..."
                            value={answers.indicators || ''}
                            onChange={e => updateAnswer('indicators', e.target.value)}
                        />
                    </div>
                );
            case 9:
                return (
                    <div className="space-y-4">
                        <Label>Customize Indicator Settings</Label>
                        <Textarea
                            placeholder="Describe any custom settings or if they should be dynamic..."
                            value={answers.indicator_settings || ''}
                            onChange={e => updateAnswer('indicator_settings', e.target.value)}
                        />
                    </div>
                );
            case 10:
                return (
                    <div className="space-y-4">
                        <Label>Data Pre-load / Warm-up Period</Label>
                        <Input
                            placeholder="e.g. 200 bars, 5 days..."
                            value={answers.data_preload || ''}
                            onChange={e => updateAnswer('data_preload', e.target.value)}
                        />
                    </div>
                );
            case 11:
                return (
                    <div className="space-y-4">
                        <Label>Schedule / Execution Times</Label>
                        <Textarea
                            placeholder="e.g. 9:30 AM - 4:00 PM EST, Mon-Fri..."
                            value={answers.schedule || ''}
                            onChange={e => updateAnswer('schedule', e.target.value)}
                        />
                    </div>
                );
            case 12:
                return (
                    <div className="space-y-4">
                        <Label>Signal Logic Description</Label>
                        <p className="text-xs text-muted-foreground">Describe exactly what triggers the entry.</p>
                        <Textarea
                            className="h-32"
                            placeholder="e.g. Buy when RSI < 30 and Price > EMA 200..."
                            value={answers.signal_logic || ''}
                            onChange={e => updateAnswer('signal_logic', e.target.value)}
                        />
                    </div>
                );
            case 13:
                return (
                    <div className="space-y-4">
                        <Label>Entry, Exit, Stop, & Target Criteria</Label>
                        <p className="text-xs text-muted-foreground">Specify types (Market/Limit), Stop Loss size, Profit Targets, Trailing Stops.</p>
                        <Textarea
                            className="h-32"
                            placeholder="e.g. Market Entry, 20 tick Stop, 40 tick Target, 10 tick Trailing Stop..."
                            value={answers.components || ''}
                            onChange={e => updateAnswer('components', e.target.value)}
                        />
                    </div>
                );
            case 14:
                return (
                    <div className="space-y-4">
                        <Label>Will this strategy use static or dynamic position sizing?</Label>
                        <Select onValueChange={(v) => updateAnswer('sizing', v)} defaultValue={answers.sizing}>
                            <SelectTrigger><SelectValue placeholder="Select Sizing Model" /></SelectTrigger>
                            <SelectContent>
                                <SelectItem value="Static">Static (Fixed Lots/Shares)</SelectItem>
                                <SelectItem value="Dynamic">Dynamic (% Risk, Volatility Adjusted)</SelectItem>
                            </SelectContent>
                        </Select>
                        <Textarea
                            placeholder="Additional details (e.g. 1 contract per $10k, or Risk 1% of Equity)..."
                            value={answers.sizing_details || ''}
                            onChange={e => updateAnswer('sizing_details', e.target.value)}
                        />
                    </div>
                );
            case 15:
                return (
                    <div className="space-y-4">
                        <Label>Create corresponding Backtest?</Label>
                        <RadioGroup onValueChange={(v) => updateAnswer('backtest', v)} defaultValue={answers.backtest}>
                            <div className="flex items-center space-x-2"><RadioGroupItem value="YES" id="bt1" /><Label htmlFor="bt1">Yes</Label></div>
                            <div className="flex items-center space-x-2"><RadioGroupItem value="NO" id="bt2" /><Label htmlFor="bt2">No</Label></div>
                        </RadioGroup>
                        {answers.backtest === 'YES' && (
                            <Input
                                placeholder="Specify symbols/timeframes for backtest..."
                                value={answers.backtest_details || ''}
                                onChange={e => updateAnswer('backtest_details', e.target.value)}
                            />
                        )}
                    </div>
                );
            case 16:
                return (
                    <div className="space-y-4">
                        <Label>Strategy Name</Label>
                        <Input
                            placeholder="MyAwesomeStrategy"
                            value={answers.naming || ''}
                            onChange={e => updateAnswer('naming', e.target.value)}
                        />
                    </div>
                );
            case 17:
                return (
                    <div className="space-y-4">
                        <Label>Custom Questions / Instructions</Label>
                        <p className="text-xs text-muted-foreground">Add any extra context for the Agent.</p>
                        <div className="space-y-2">
                            {(answers.customQuestions || []).map((q: string, idx: number) => (
                                <div key={idx} className="flex gap-2">
                                    <Input value={q} readOnly />
                                    <Button variant="ghost" size="icon" onClick={() => {
                                        const newQ = [...answers.customQuestions];
                                        newQ.splice(idx, 1);
                                        updateAnswer('customQuestions', newQ);
                                    }}><Trash2 className="w-4 h-4" /></Button>
                                </div>
                            ))}
                        </div>
                        <div className="flex gap-2">
                            <Input id="new-q" placeholder="Add custom instruction..."
                                onKeyDown={e => {
                                    if (e.key === 'Enter') {
                                        const val = (e.target as HTMLInputElement).value;
                                        if (val) {
                                            updateAnswer('customQuestions', [...(answers.customQuestions || []), val]);
                                            (e.target as HTMLInputElement).value = '';
                                        }
                                    }
                                }}
                            />
                            <Button variant="secondary" onClick={() => {
                                const el = document.getElementById('new-q') as HTMLInputElement;
                                if (el && el.value) {
                                    updateAnswer('customQuestions', [...(answers.customQuestions || []), el.value]);
                                    el.value = '';
                                }
                            }}><Plus className="w-4 h-4" /></Button>
                        </div>
                    </div>
                );
            default:
                return null;
        }
    };

    return (
        <Card className="h-full border-0 shadow-none flex flex-col bg-transparent">
            <CardHeader className="px-4 py-2 border-b">
                <CardTitle className="text-sm font-medium flex items-center justify-between">
                    <span>{currentStepConfig?.title}</span>
                    <span className="text-xs text-muted-foreground">Step {step} of {steps.length}</span>
                </CardTitle>
                <div className="w-full bg-secondary h-1 mt-2 rounded-full overflow-hidden">
                    <div className="bg-primary h-full transition-all duration-300" style={{ width: `${(step / steps.length) * 100}%` }} />
                </div>
            </CardHeader>
            <CardContent className="flex-1 overflow-y-auto p-4">
                {renderStepContent()}
            </CardContent>
            <CardFooter className="px-4 py-2 border-t flex justify-between bg-background/50">
                <Button variant="ghost" onClick={prevStep} disabled={step === 1} size="sm">
                    <ChevronLeft className="w-4 h-4 mr-1" /> Back
                </Button>
                {step < steps.length ? (
                    <Button onClick={nextStep} size="sm">
                        Next <ChevronRight className="w-4 h-4 ml-1" />
                    </Button>
                ) : (
                    <Button onClick={() => onComplete(answers)} size="sm" className="bg-green-600 hover:bg-green-700">
                        Generate Strategy <Check className="w-4 h-4 ml-1" />
                    </Button>
                )}
            </CardFooter>
        </Card>
    );
};
