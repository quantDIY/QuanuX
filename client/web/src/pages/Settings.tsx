import React, { useEffect, useState } from 'react';
import { Button } from '@quanux/shared/components/ui/button';
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from '@quanux/shared/components/ui/card';
import { Input } from '@quanux/shared/components/ui/input';
import { Alert, AlertDescription, AlertTitle } from '@quanux/shared/components/ui/alert';
import { Copy, Plus, RefreshCw, Trash2, Key } from 'lucide-react';
import { useToast } from '@quanux/shared/hooks/use-toast';

interface SecretStatus {
    key: string;
    label: string;
    is_set: boolean;
}

export const Settings: React.FC = () => {
    const { toast } = useToast();
    const [secrets, setSecrets] = useState<SecretStatus[]>([]);
    const [loading, setLoading] = useState(true);
    const [generatedKey, setGeneratedKey] = useState<{ key: string, value: string } | null>(null);

    const fetchSecrets = async () => {
        setLoading(true);
        try {
            // In a real app, use a configured base URL. For scaffold, assume proxy or direct.
            const res = await fetch('http://localhost:8000/api/secrets/list');
            if (!res.ok) throw new Error("Failed to fetch secrets");
            const data = await res.json();
            setSecrets(data);
        } catch (error) {
            console.error(error);
            toast({
                title: "Error",
                description: "Could not load integration status.",
                variant: "destructive"
            });
        } finally {
            setLoading(false);
        }
    };

    useEffect(() => {
        fetchSecrets();
    }, []);

    const generateLocalKey = (keyName: string, label: string) => {
        // For privacy, we generate the key HERE in the browser (or request backend to do it securely).
        // Since backend stores it, we should ask backend to generating it for cryptographic randomness.
        // Actually, for "User API Keys", the User generates it and sets it.
        // BUT for "Bridge Keys" (e.g. n8n), we want the Server to respect it.
        // Simplest Privacy flow: 
        // 1. User clicks "Generate New Key".
        // 2. Browser generates random string.
        // 3. Browser sends string to Server (/api/secrets).
        // 4. Browser shows key to User ONCE.

        const array = new Uint8Array(32);
        window.crypto.getRandomValues(array);
        const newKey = Array.from(array, byte => byte.toString(16).padStart(2, '0')).join('');

        // Save to server
        fetch('http://localhost:8000/api/secrets', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ key: keyName, value: newKey })
        }).then(res => {
            if (res.ok) {
                setGeneratedKey({ key: label, value: newKey });
                fetchSecrets();
                toast({ title: "Success", description: "New key generated and saved." });
            } else {
                toast({ title: "Error", description: "Failed to save key.", variant: "destructive" });
            }
        });
    };

    const copyToClipboard = (text: string) => {
        navigator.clipboard.writeText(text);
        toast({ title: "Copied!", description: "Key copied to clipboard." });
    };

    const revokeKey = async (keyName: string) => {
        if (!confirm("Are you sure? This will break the integration immediately.")) return;

        // We overload POST with empty value to 'unset' or delete
        // In a real API we'd use DELETE, but our simple endpoint is POST-only for set.
        // Let's assume sending empty string clears it or we need to implement DELETE.
        // For MVP scaffold, we'll just set it to empty string.
        try {
            const res = await fetch('http://localhost:8000/api/secrets', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ key: keyName, value: "" })
            });
            if (res.ok) {
                toast({ title: "Revoked", description: "Key has been revoked." });
                fetchSecrets();
            }
        } catch (e) {
            toast({ title: "Error", description: "Failed to revoke key.", variant: "destructive" });
        }
    };

    return (
        <div className="space-y-8 animate-in fade-in slide-in-from-bottom-4 duration-500">
            <div>
                <h2 className="text-3xl font-bold tracking-tight">Settings</h2>
                <p className="text-muted-foreground">Manage your local API keys and external integrations.</p>
            </div>

            {/* Generated Key Modal / Alert */}
            {generatedKey && (
                <Alert className="border-green-500/50 bg-green-500/10 text-green-500">
                    <Key className="h-4 w-4" />
                    <AlertTitle>New API Key Generated</AlertTitle>
                    <AlertDescription className="mt-2 text-foreground">
                        <p className="mb-2">Here is your new key for <strong>{generatedKey.key}</strong>. Copy it now, you won't see it again.</p>
                        <div className="flex items-center gap-2">
                            <Input value={generatedKey.value} readOnly className="font-mono bg-background" />
                            <Button size="icon" variant="outline" onClick={() => copyToClipboard(generatedKey.value)}>
                                <Copy className="h-4 w-4" />
                            </Button>
                        </div>
                    </AlertDescription>
                </Alert>
            )}

            <div className="grid gap-6">
                <Card>
                    <CardHeader>
                        <CardTitle>QuanuX Extensions (Bolt-ons)</CardTitle>
                        <CardDescription>
                            Manage keys for local sidecars like n8n, Figma, etc.
                            These keys allow external apps to talk to your QuanuX Core.
                        </CardDescription>
                    </CardHeader>
                    <CardContent className="space-y-4">
                        {secrets.filter(s => s.key.startsWith("QUANUX_N8N") || s.key.includes("BRIDGE")).map(secret => (
                            <div key={secret.key} className="flex items-center justify-between p-4 rounded-lg border border-border bg-card/50">
                                <div className="flex items-center gap-4">
                                    <div className={`w-3 h-3 rounded-full ${secret.is_set ? 'bg-green-500 shadow-[0_0_8px_#00ff00]' : 'bg-red-500'}`} />
                                    <div>
                                        <h4 className="font-semibold">{secret.label}</h4>
                                        <p className="text-xs text-muted-foreground font-mono">{secret.key}</p>
                                    </div>
                                </div>
                                <div className="flex items-center gap-2">
                                    {secret.is_set ? (
                                        <Button variant="ghost" size="sm" className="text-destructive hover:text-destructive/80" onClick={() => revokeKey(secret.key)}>
                                            <Trash2 className="w-4 h-4 mr-2" /> Revoke
                                        </Button>
                                    ) : (
                                        <Button variant="outline" size="sm" onClick={() => generateLocalKey(secret.key, secret.label)}>
                                            <Plus className="w-4 h-4 mr-2" /> Generate Key
                                        </Button>
                                    )}
                                </div>
                            </div>
                        ))}
                    </CardContent>
                </Card>

                <Card>
                    <CardHeader>
                        <CardTitle>External Integrations</CardTitle>
                        <CardDescription>
                            Connection status for Prop Firms, Brokers, and Data Feeds.
                        </CardDescription>
                    </CardHeader>
                    <CardContent className="space-y-4">
                        {secrets.filter(s => !s.key.startsWith("QUANUX_N8N") && !s.key.includes("BRIDGE")).map(secret => (
                            <div key={secret.key} className="flex items-center justify-between p-4 rounded-lg border border-border bg-card/50">
                                <div className="flex items-center gap-4">
                                    <div className={`w-2 h-2 rounded-full ${secret.is_set ? 'bg-green-500' : 'bg-muted-foreground'}`} />
                                    <span className="font-medium">{secret.label}</span>
                                </div>
                                <div>
                                    {secret.is_set ? (
                                        <span className="text-xs bg-green-500/10 text-green-500 px-2 py-1 rounded border border-green-500/20">Connected</span>
                                    ) : (
                                        <span className="text-xs text-muted-foreground">Not Configured</span>
                                    )}
                                    {/* Edit functionality omitted for brevity in pilot */}
                                </div>
                            </div>
                        ))}
                    </CardContent>
                </Card>
            </div>
        </div>
    );
};
