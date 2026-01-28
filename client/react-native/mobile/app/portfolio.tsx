import { View, Text, ScrollView } from 'react-native';
import { Card, CardContent, CardHeader, CardTitle } from '@quanux/ui';
import { Chart } from '@quanux/ui';

export default function PortfolioScreen() {
    return (
        <ScrollView className="flex-1 bg-background p-4 pt-12">
            <Text className="mb-6 text-3xl font-bold text-foreground">Portfolio</Text>

            <Card className="mb-6 border-primary/20 bg-primary/5">
                <CardHeader>
                    <CardTitle className="text-primary">Total Value</CardTitle>
                </CardHeader>
                <CardContent>
                    <Text className="text-4xl font-bold text-foreground">$124,592.40</Text>
                    <Text className="text-green-500 font-semibold">+$2,402 (1.92%) Today</Text>
                </CardContent>
            </Card>

            <Chart className="mb-6" title="Performance" />

            <Text className="mb-4 text-lg font-semibold text-foreground">Asset Allocation</Text>
            <View className="flex-row flex-wrap gap-2">
                <View className="h-24 w-1/2 flex-1 rounded-lg bg-blue-500/20 p-3">
                    <Text className="text-foreground font-bold">Stocks</Text>
                    <Text className="text-2xl text-foreground">60%</Text>
                </View>
                <View className="h-24 w-1/2 flex-1 rounded-lg bg-purple-500/20 p-3">
                    <Text className="text-foreground font-bold">Crypto</Text>
                    <Text className="text-2xl text-foreground">30%</Text>
                </View>
            </View>
        </ScrollView>
    );
}
