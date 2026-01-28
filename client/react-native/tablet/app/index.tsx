import { useState } from 'react';
import { View, Text, ScrollView, FlatList, TouchableOpacity } from 'react-native';
import { Card, CardContent, CardHeader, CardTitle, CardDescription } from '@quanux/ui';
import { Chart, Badge } from '@quanux/ui';
import { cn } from '@quanux/ui';

const STOCK_DATA = [
    { symbol: 'AAPL', name: 'Apple Inc.', price: '185.92', change: '+0.45%', description: 'Technology Company' },
    { symbol: 'MSFT', name: 'Microsoft Corp.', price: '402.56', change: '+1.20%', description: 'Software Giant' },
    { symbol: 'NVDA', name: 'NVIDIA Corp.', price: '596.54', change: '+2.30%', description: 'AI Chipmaker' },
    { symbol: 'GOOGL', name: 'Alphabet Inc.', price: '152.12', change: '-0.15%', description: 'Search Engine' },
    { symbol: 'AMZN', name: 'Amazon.com', price: '174.42', change: '+0.88%', description: 'E-commerce' },
];

export default function TabletDashboard() {
    const [selectedStock, setSelectedStock] = useState(STOCK_DATA[0]);

    return (
        <View className="flex-1 flex-row bg-background">
            {/* Sidebar List (Left Pane) */}
            <View className="w-1/3 border-r border-border bg-card/50">
                <View className="p-6 border-b border-border">
                    <Text className="text-2xl font-bold text-foreground">Markets</Text>
                    <Text className="text-muted-foreground">Watchlist</Text>
                </View>
                <FlatList
                    data={STOCK_DATA}
                    keyExtractor={(item) => item.symbol}
                    renderItem={({ item }) => (
                        <TouchableOpacity
                            onPress={() => setSelectedStock(item)}
                            className={cn(
                                "border-b border-border p-4 transition-all hover:bg-muted/50",
                                selectedStock.symbol === item.symbol ? "bg-muted" : "bg-transparent"
                            )}
                        >
                            <View className="flex-row justify-between items-center">
                                <View>
                                    <Text className="font-bold text-foreground">{item.symbol}</Text>
                                    <Text className="text-xs text-muted-foreground">{item.name}</Text>
                                </View>
                                <View className="items-end">
                                    <Text className="font-medium text-foreground">${item.price}</Text>
                                    <Badge variant={item.change.startsWith('+') ? 'default' : 'destructive'}>
                                        {item.change}
                                    </Badge>
                                </View>
                            </View>
                        </TouchableOpacity>
                    )}
                />
            </View>

            {/* Main Content (Right Pane) */}
            <View className="flex-1 p-8 bg-background">
                <View className="mb-8 flex-row items-start justify-between">
                    <View>
                        <Text className="text-4xl font-bold text-foreground">{selectedStock.symbol}</Text>
                        <Text className="text-xl text-muted-foreground">{selectedStock.name}</Text>
                        <Text className="mt-2 text-sm text-muted-foreground max-w-md">
                            {selectedStock.description}. A leading company in the sector showing strong momentum.
                        </Text>
                    </View>
                    <View className="items-end">
                        <Text className="text-5xl font-bold text-foreground">${selectedStock.price}</Text>
                        <Text className={cn("text-xl font-medium", selectedStock.change.startsWith('+') ? "text-green-500" : "text-red-500")}>
                            {selectedStock.change} Today
                        </Text>
                    </View>
                </View>

                <Chart title={`${selectedStock.symbol} Performance`} className="flex-1 mb-8" />

                <View className="flex-row gap-4">
                    <Card className="flex-1">
                        <CardHeader><CardTitle>Volume</CardTitle></CardHeader>
                        <CardContent><Text className="text-2xl font-bold text-foreground">42.5M</Text></CardContent>
                    </Card>
                    <Card className="flex-1">
                        <CardHeader><CardTitle>Market Cap</CardTitle></CardHeader>
                        <CardContent><Text className="text-2xl font-bold text-foreground">2.8T</Text></CardContent>
                    </Card>
                    <Card className="flex-1">
                        <CardHeader><CardTitle>P/E Ratio</CardTitle></CardHeader>
                        <CardContent><Text className="text-2xl font-bold text-foreground">32.4</Text></CardContent>
                    </Card>
                </View>
            </View>
        </View>
    );
}
