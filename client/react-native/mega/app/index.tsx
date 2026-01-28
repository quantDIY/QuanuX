import { View, Text, ScrollView } from 'react-native';
import { Card, CardHeader, CardTitle, CardContent } from '@quanux/ui';
import { Chart } from '@quanux/ui';
import { Separator } from '@quanux/ui';
import '../../global.css';

export default function MegaDashboard() {
    return (
        <View className="flex-1 bg-background p-8">
            <View className="mb-6 flex-row items-center justify-between">
                <Text className="text-4xl font-bold text-foreground">Global Markets Dashboard</Text>
                <Text className="text-xl text-muted-foreground">Live Feed • NYC</Text>
            </View>
            <Separator orientation="horizontal" className="mb-6" />

            {/* Grid Layout */}
            <View className="flex-1 flex-row flex-wrap gap-6">
                {/* Main Chart (Span 2 col) */}
                <View className="w-[65%] h-[400px]">
                    <Chart title="S&P 500 Intraday" className="h-full" />
                </View>

                {/* Side Panel (Span 1 col) */}
                <View className="flex-1 h-[400px] gap-4">
                    <Card className="flex-1 bg-card">
                        <CardHeader><CardTitle>Top Volume</CardTitle></CardHeader>
                        <CardContent><Text className="text-foreground">AAPL: 45M</Text></CardContent>
                    </Card>
                    <Card className="flex-1 bg-card">
                        <CardHeader><CardTitle>Volatility</CardTitle></CardHeader>
                        <CardContent><Text className="text-foreground">VIX: 12.4</Text></CardContent>
                    </Card>
                </View>
            </View>

            {/* Bottom Row */}
            <View className="flex-row gap-6 mt-6 h-[250px]">
                <Card className="flex-1"><CardHeader><CardTitle>Forex</CardTitle></CardHeader></Card>
                <Card className="flex-1"><CardHeader><CardTitle>Crypto</CardTitle></CardHeader></Card>
                <Card className="flex-1"><CardHeader><CardTitle>Commodities</CardTitle></CardHeader></Card>
                <Card className="flex-1"><CardHeader><CardTitle>Futures</CardTitle></CardHeader></Card>
            </View>
        </View>
    );
}
