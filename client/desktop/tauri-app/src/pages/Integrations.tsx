
import React, { useState } from 'react';
import { Button } from '@quanux/shared/components/ui/button';
import { Card, CardContent, CardDescription, CardFooter, CardHeader, CardTitle } from '@quanux/shared/components/ui/card';
import { Input } from '@quanux/shared/components/ui/input';
import { Label } from '@quanux/shared/components/ui/label';
import { useToast } from '@quanux/shared/hooks/use-toast';
import { Plug, Check, Key, Lock, Loader2 } from 'lucide-react';

interface IntegrationCardProps {
    id: string;
    name: string;
    description: string;
    icon: React.ReactNode;
    fields: { key: string; label: string; placeholder: string }[];
    connectUrl?: string; // URL to start OAuth flow
    isConnected?: boolean;
}

export const Integrations = () => {
    const { toast } = useToast();
    const [connectingId, setConnectingId] = useState<string | null>(null);
    const [secrets, setSecrets] = useState<Record<string, string>>({});

    // Check for success param on load
    React.useEffect(() => {
        const params = new URLSearchParams(window.location.search);
        if (params.get('success') === 'true') {
            const provider = params.get('provider');
            toast({
                title: "Integration Connected",
                description: `Successfully authenticated with ${provider}.`,
            });
            // Clean URL
            window.history.replaceState({}, '', window.location.pathname);
        }
    }, [toast]);

    // --- Categories ---
    const brokers: IntegrationCardProps[] = [
        {
            id: 'ironbeam',
            name: 'Ironbeam Futures',
            description: 'Direct DMA access via Rithmic.',
            icon: <Plug className="w-8 h-8 text-blue-600" />,
            fields: [
                { key: 'QUANUX_IRONBEAM_USER', label: 'Username', placeholder: 'user' },
                { key: 'QUANUX_IRONBEAM_PASS', label: 'Password', placeholder: 'password' },
                { key: 'QUANUX_IRONBEAM_SYSTEM', label: 'System', placeholder: 'Ironbeam' },
                { key: 'QUANUX_IRONBEAM_URL', label: 'WSS URL', placeholder: 'wss://...' },
                { key: 'QUANUX_IRONBEAM_APP_NAME', label: 'App Name', placeholder: 'QuanuX' },
                { key: 'QUANUX_IRONBEAM_APP_VERSION', label: 'App Version', placeholder: '17.9.0.0' },
            ]
        },
        {
            id: 'amp',
            name: 'Amp Futures',
            description: 'Low margin futures trading.',
            icon: <Plug className="w-8 h-8 text-blue-500" />,
            fields: [
                { key: 'QUANUX_AMP_USER', label: 'Username', placeholder: 'user' },
                { key: 'QUANUX_AMP_PASS', label: 'Password', placeholder: 'password' },
                { key: 'QUANUX_AMP_SYSTEM', label: 'System', placeholder: 'Amp' },
                { key: 'QUANUX_AMP_URL', label: 'WSS URL', placeholder: 'wss://...' },
                { key: 'QUANUX_AMP_APP_NAME', label: 'App Name', placeholder: 'QuanuX' },
                { key: 'QUANUX_AMP_APP_VERSION', label: 'App Version', placeholder: '17.9.0.0' },
            ]
        },
        {
            id: 'discount',
            name: 'Discount Trading',
            description: 'Deep discount futures broker.',
            icon: <Plug className="w-8 h-8 text-green-600" />,
            fields: [
                { key: 'QUANUX_DISCOUNT_USER', label: 'Username', placeholder: 'user' },
                { key: 'QUANUX_DISCOUNT_PASS', label: 'Password', placeholder: 'password' },
                { key: 'QUANUX_DISCOUNT_SYSTEM', label: 'System', placeholder: 'Discount' },
                { key: 'QUANUX_DISCOUNT_URL', label: 'WSS URL', placeholder: 'wss://...' },
                { key: 'QUANUX_DISCOUNT_APP_NAME', label: 'App Name', placeholder: 'QuanuX' },
                { key: 'QUANUX_DISCOUNT_APP_VERSION', label: 'App Version', placeholder: '17.9.0.0' },
            ]
        }
    ];

    const propFirms: IntegrationCardProps[] = [
        {
            id: 'topstep',
            name: 'Topstep',
            description: 'Leading prop firm (API & Rithmic).',
            icon: <Plug className="w-8 h-8 text-blue-500" />,
            fields: [
                { key: 'TOPSTEP__USERNAME', label: 'Username', placeholder: 'user' },
                { key: 'TOPSTEP__PASSWORD', label: 'Password', placeholder: 'password' },
                { key: 'TOPSTEP__API_KEY', label: 'API Key', placeholder: 'ts-...' }
            ],
            connectUrl: '/api/auth/topstep/start'
        },
        {
            id: 'apex',
            name: 'Apex Trader',
            description: 'Popular funding evaluation firm.',
            icon: <Plug className="w-8 h-8 text-red-600" />,
            fields: [
                { key: 'QUANUX_APEX_USER', label: 'Username', placeholder: 'user' },
                { key: 'QUANUX_APEX_PASS', label: 'Password', placeholder: 'password' },
                { key: 'QUANUX_APEX_SYSTEM', label: 'System', placeholder: 'Apex' },
                { key: 'QUANUX_APEX_URL', label: 'WSS URL', placeholder: 'wss://...' },
                { key: 'QUANUX_APEX_APP_NAME', label: 'App Name', placeholder: 'QuanuX' },
                { key: 'QUANUX_APEX_APP_VERSION', label: 'App Version', placeholder: '17.9.0.0' },
            ]
        },
        {
            id: 'elite',
            name: 'Elite Trader',
            description: 'Flexible funding options.',
            icon: <Plug className="w-8 h-8 text-yellow-500" />,
            fields: [
                { key: 'QUANUX_ELITE_USER', label: 'Username', placeholder: 'user' },
                { key: 'QUANUX_ELITE_PASS', label: 'Password', placeholder: 'password' },
                { key: 'QUANUX_ELITE_SYSTEM', label: 'System', placeholder: 'Elite' },
                { key: 'QUANUX_ELITE_URL', label: 'WSS URL', placeholder: 'wss://...' },
                { key: 'QUANUX_ELITE_APP_NAME', label: 'App Name', placeholder: 'QuanuX' },
                { key: 'QUANUX_ELITE_APP_VERSION', label: 'App Version', placeholder: '17.9.0.0' },
            ]
        },
        {
            id: 'takeprofit',
            name: 'Take Profit Trader',
            description: 'Streamlined funding process.',
            icon: <Plug className="w-8 h-8 text-green-500" />,
            fields: [
                { key: 'QUANUX_TAKEPROFIT_USER', label: 'Username', placeholder: 'user' },
                { key: 'QUANUX_TAKEPROFIT_PASS', label: 'Password', placeholder: 'password' },
                { key: 'QUANUX_TAKEPROFIT_SYSTEM', label: 'System', placeholder: 'TakeProfit' },
                { key: 'QUANUX_TAKEPROFIT_URL', label: 'WSS URL', placeholder: 'wss://...' },
                { key: 'QUANUX_TAKEPROFIT_APP_NAME', label: 'App Name', placeholder: 'QuanuX' },
                { key: 'QUANUX_TAKEPROFIT_APP_VERSION', label: 'App Version', placeholder: '17.9.0.0' },
            ]
        }
    ];

    const chartingAndTrading: IntegrationCardProps[] = [
        {
            id: 'tradingview',
            name: 'TradingView',
            description: 'Advanced Charting (Coming Soon)',
            icon: <Plug className="w-8 h-8 text-orange-500" />,
            fields: [{ key: 'TRADINGVIEW_CLIENT_ID', label: 'Client ID', placeholder: 'Client ID' }]
        },
        {
            id: 'edgeprox',
            name: 'EdgeProX',
            description: 'Professional trading platform.',
            icon: <Plug className="w-8 h-8 text-indigo-500" />,
            fields: [
                { key: 'QUANUX_EDGEPROX_USER', label: 'Username', placeholder: 'user' },
                { key: 'QUANUX_EDGEPROX_PASS', label: 'Password', placeholder: 'password' },
            ]
        },
        {
            id: 'motivewave',
            name: 'MotiveWave',
            description: 'Elliot Wave software.',
            icon: <Plug className="w-8 h-8 text-blue-400" />,
            fields: [
                { key: 'QUANUX_MOTIVEWAVE_USER', label: 'Username', placeholder: 'user' },
                { key: 'QUANUX_MOTIVEWAVE_PASS', label: 'Password', placeholder: 'password' },
            ]
        },
        {
            id: 'sierra',
            name: 'Sierra Chart',
            description: 'High-performance charting.',
            icon: <Plug className="w-8 h-8 text-green-700" />,
            fields: [
                { key: 'QUANUX_SIERRA_USER', label: 'Username', placeholder: 'user' },
                { key: 'QUANUX_SIERRA_PASS', label: 'Password', placeholder: 'password' },
            ]
        },
        {
            id: 'jigsaw',
            name: 'Jigsaw Trading',
            description: 'Order flow tools.',
            icon: <Plug className="w-8 h-8 text-red-500" />,
            fields: [
                { key: 'QUANUX_JIGSAW_USER', label: 'Username', placeholder: 'user' },
                { key: 'QUANUX_JIGSAW_PASS', label: 'Password', placeholder: 'password' },
            ]
        },
        {
            id: 'quantower',
            name: 'Quantower',
            description: 'Multi-asset trading platform.',
            icon: <Plug className="w-8 h-8 text-purple-600" />,
            fields: [
                { key: 'QUANUX_QUANTOWER_USER', label: 'Username', placeholder: 'user' },
                { key: 'QUANUX_QUANTOWER_PASS', label: 'Password', placeholder: 'password' },
            ]
        },
        {
            id: 'investorrt',
            name: 'Investor/RT',
            description: 'Volume profile platform.',
            icon: <Plug className="w-8 h-8 text-yellow-600" />,
            fields: [
                { key: 'QUANUX_INVESTORRT_USER', label: 'Username', placeholder: 'user' },
                { key: 'QUANUX_INVESTORRT_PASS', label: 'Password', placeholder: 'password' },
            ]
        },
        {
            id: 'multicharts',
            name: 'MultiCharts',
            description: 'Automated trading software.',
            icon: <Plug className="w-8 h-8 text-blue-800" />,
            fields: [
                { key: 'QUANUX_MULTICHARTS_USER', label: 'Username', placeholder: 'user' },
                { key: 'QUANUX_MULTICHARTS_PASS', label: 'Password', placeholder: 'password' },
            ]
        },
        {
            id: 'tradenav',
            name: 'Trade Navigator',
            description: 'Technical analysis software.',
            icon: <Plug className="w-8 h-8 text-teal-600" />,
            fields: [
                { key: 'QUANUX_TRADENAV_USER', label: 'Username', placeholder: 'user' },
                { key: 'QUANUX_TRADENAV_PASS', label: 'Password', placeholder: 'password' },
            ]
        },
        {
            id: 'bookmap',
            name: 'Bookmap',
            description: 'Visual order book platform.',
            icon: <Plug className="w-8 h-8 text-indigo-400" />,
            fields: [
                { key: 'QUANUX_BOOKMAP_USER', label: 'Username', placeholder: 'user' },
                { key: 'QUANUX_BOOKMAP_PASS', label: 'Password', placeholder: 'password' },
            ]
        },
    ];

    const dataProviders: IntegrationCardProps[] = [
        {
            id: 'rithmic',
            name: 'Rithmic (Generic)',
            description: 'Direct Rithmic Protocol connection.',
            icon: <Plug className="w-8 h-8 text-red-500" />,
            fields: [
                { key: 'QUANUX_RITHMIC_USER', label: 'Username', placeholder: 'user_123' },
                { key: 'QUANUX_RITHMIC_PASS', label: 'Password', placeholder: 'password' },
                { key: 'QUANUX_RITHMIC_SYSTEM', label: 'System', placeholder: 'Rithmic Test' },
                { key: 'QUANUX_RITHMIC_URL', label: 'WSS URL', placeholder: 'wss://...' },
                { key: 'QUANUX_RITHMIC_APP_NAME', label: 'App Name', placeholder: 'QuanuX' },
                { key: 'QUANUX_RITHMIC_APP_VERSION', label: 'App Version', placeholder: '17.9.0.0' },
            ]
        },
        {
            id: 'openai',
            name: 'OpenAI',
            description: 'Strategy generation with GPT-4.',
            icon: <Plug className="w-8 h-8 text-green-500" />,
            fields: [{ key: 'OPENAI_API_KEY', label: 'API Key', placeholder: 'sk-...' }],
            // Mock OAuth for OpenAI just to demonstrate the flow requested
            connectUrl: '/api/auth/openai/start'
        },
        {
            id: 'gemini',
            name: 'Google Gemini',
            description: 'High-speed strategy generation.',
            icon: <Plug className="w-8 h-8 text-purple-500" />,
            fields: [{ key: 'QUANUX_GEMINI_API_KEY', label: 'API Key', placeholder: 'AIza...' }]
        },
        {
            id: 'databento',
            name: 'Databento',
            description: 'Historical market data (nanosecond resolution).',
            icon: <Plug className="w-8 h-8 text-orange-600" />,
            fields: [{ key: 'DATABENTO_API_KEY', label: 'API Key', placeholder: 'db-...' }]
        },
        {
            id: 'motherduck',
            name: 'MotherDuck',
            description: 'Serverless DuckDB in the cloud.',
            icon: <Plug className="w-8 h-8 text-yellow-400" />,
            fields: [{ key: 'MOTHERDUCK_TOKEN', label: 'Service Token', placeholder: 'md-...' }]
        },
    ];

    const handleOAuth = (url: string) => {
        window.location.href = `http://localhost:8080${url}`;
    };

    const handleSave = async (provider: IntegrationCardProps) => {
        setConnectingId(provider.id);

        try {
            // Save each field
            for (const field of provider.fields) {
                const value = secrets[field.key];
                if (!value) continue; // Skip empty (allows partial updates)

                const res = await fetch('/api/secrets', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/json' },
                    body: JSON.stringify({ key: field.key, value })
                });

                if (!res.ok) throw new Error(`Failed to save ${field.label}`);
            }

            toast({
                title: "Connected Successfully",
                description: `${provider.name} credentials secured.`,
            });
            setSecrets(prev => {
                const clearing = { ...prev };
                provider.fields.forEach(f => delete clearing[f.key]);
                return clearing;
            });

        } catch (error) {
            toast({
                title: "Connection Failed",
                description: String(error),
                variant: "destructive"
            });
        } finally {
            setConnectingId(null);
        }
    };

    // Helper to render a grid
    const renderGrid = (title: string, items: IntegrationCardProps[]) => (
        <div className="mb-8">
            <h2 className="text-xl font-semibold mb-4 tracking-tight">{title}</h2>
            <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6">
                {items.map(provider => (
                    <Card key={provider.id} className="flex flex-col">
                        <CardHeader>
                            <div className="flex items-center gap-4 mb-2">
                                <div className="p-2 bg-muted rounded-lg">{provider.icon}</div>
                                <div>
                                    <CardTitle>{provider.name}</CardTitle>
                                    <CardDescription>Provider</CardDescription>
                                </div>
                            </div>
                            <CardDescription>{provider.description}</CardDescription>
                        </CardHeader>
                        <CardContent className="flex-1 space-y-4">
                            {provider.connectUrl && (
                                <Button
                                    className="w-full mb-4 bg-indigo-600 hover:bg-indigo-700 text-white"
                                    onClick={() => handleOAuth(provider.connectUrl!)}
                                >
                                    Login with {provider.name}
                                </Button>
                            )}

                            {provider.fields.length > 0 && provider.connectUrl && (
                                <div className="relative mb-4">
                                    <div className="absolute inset-0 flex items-center"><span className="w-full border-t border-muted"></span></div>
                                    <div className="relative flex justify-center text-xs uppercase"><span className="bg-card px-2 text-muted-foreground">Or Enter Manually</span></div>
                                </div>
                            )}

                            {provider.fields.map(field => (
                                <div key={field.key} className="space-y-2">
                                    <Label htmlFor={field.key}>{field.label}</Label>
                                    <Input
                                        id={field.key}
                                        type="password"
                                        placeholder={field.placeholder}
                                        value={secrets[field.key] || ''}
                                        onChange={e => setSecrets(prev => ({ ...prev, [field.key]: e.target.value }))}
                                    />
                                </div>
                            ))}
                        </CardContent>
                        <CardFooter className="flex justify-between border-t p-6">
                            <div className="flex items-center text-xs text-muted-foreground gap-1">
                                <Lock className="w-3 h-3" /> Encrypted via Keyring
                            </div>
                            <Button
                                variant="outline"
                                onClick={() => handleSave(provider)}
                                disabled={connectingId === provider.id}
                            >
                                {connectingId === provider.id ? (
                                    <><Loader2 className="w-4 h-4 mr-2 animate-spin" /> Saving</>
                                ) : (
                                    "Save Keys"
                                )}
                            </Button>
                        </CardFooter>
                    </Card>
                ))}
            </div>
        </div>
    );

    return (
        <div className="h-full p-8 overflow-y-auto animate-in fade-in">
            <div className="mb-8">
                <h1 className="text-3xl font-bold tracking-tight mb-2">Integrations</h1>
                <p className="text-muted-foreground">Manage your connections to external services, brokers, and prop firms.</p>
            </div>

            {renderGrid("Brokers", brokers)}
            {renderGrid("Proprietary Trading Firms", propFirms)}
            {renderGrid("Charting and Trading", chartingAndTrading)}
            {renderGrid("Data & AI Providers", dataProviders)}
        </div>
    );
};
