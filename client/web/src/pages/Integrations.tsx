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

    const providers: IntegrationCardProps[] = [
        {
            id: 'openai',
            name: 'OpenAI',
            description: 'Connect your OpenAI API Key to generate strategies with GPT-4.',
            icon: <Plug className="w-8 h-8 text-green-500" />,
            fields: [{ key: 'OPENAI_API_KEY', label: 'API Key', placeholder: 'sk-...' }],
            // Mock OAuth for OpenAI just to demonstrate the flow requested
            connectUrl: '/api/auth/openai/start'
        },
        {
            id: 'gemini',
            name: 'Google Gemini',
            description: 'Use Google\'s Gemini Flash for high-speed strategy generation.',
            icon: <Plug className="w-8 h-8 text-purple-500" />,
            fields: [{ key: 'QUANUX_GEMINI_API_KEY', label: 'API Key', placeholder: 'AIza...' }]
        },
        {
            id: 'topstep',
            name: 'Topstep',
            description: 'Connect your Topstep account for live trading and data.',
            icon: <Plug className="w-8 h-8 text-blue-500" />,
            fields: [
                { key: 'TOPSTEP__USERNAME', label: 'Username', placeholder: 'user@example.com' },
                { key: 'TOPSTEP__PASSWORD', label: 'Password', placeholder: 'password' },
                { key: 'TOPSTEP__API_KEY', label: 'API Key', placeholder: 'ts-...' }
            ],
            connectUrl: '/api/auth/topstep/start'
        },
        {
            id: 'tradingview',
            name: 'TradingView',
            description: 'Integration for charting data (Coming Verification).',
            icon: <Plug className="w-8 h-8 text-orange-500" />,
            fields: [{ key: 'TRADINGVIEW_CLIENT_ID', label: 'Client ID', placeholder: 'Client ID' }]
        }
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
                if (!value) continue; // Skip empty

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

    return (
        <div className="h-full p-8 overflow-y-auto animate-in fade-in">
            <div className="mb-8">
                <h1 className="text-3xl font-bold tracking-tight mb-2">Integrations</h1>
                <p className="text-muted-foreground">Manage your connections to external services and brokers.</p>
            </div>

            <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6">
                {providers.map(provider => (
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

                            <div className="relative mb-4">
                                <div className="absolute inset-0 flex items-center"><span className="w-full border-t border-muted"></span></div>
                                <div className="relative flex justify-center text-xs uppercase"><span className="bg-card px-2 text-muted-foreground">Or Enter Manually</span></div>
                            </div>

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
};
