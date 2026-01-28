import { View, Text, ScrollView, FlatList } from 'react-native';
import { Card, CardHeader, CardTitle, CardContent } from '@quanux/ui';
import { Badge } from '@quanux/ui';

// Mock Data
const WATCHLIST = [
    { symbol: 'AAPL', name: 'Apple Inc.', price: '185.92', change: '+0.45%' },
    { symbol: 'MSFT', name: 'Microsoft', price: '402.56', change: '+1.20%' },
    { symbol: 'GOOGL', name: 'Alphabet', price: '152.12', change: '-0.15%' },
    { symbol: 'AMZN', name: 'Amazon', price: '174.42', change: '+0.88%' },
];

export default function WatchlistScreen() {
    return (
        <View className="flex-1 bg-background p-4 pt-12">
            <Text className="mb-6 text-3xl font-bold text-foreground">Watchlist</Text>

            <FlatList
                data={WATCHLIST}
                keyExtractor={(item) => item.symbol}
                renderItem={({ item }) => (
                    <Card className="mb-3">
                        <View className="flex-row items-center justify-between p-4">
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
                    </Card>
                )}
            />
        </View>
    );
}
