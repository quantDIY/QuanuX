import { View, Text, ScrollView } from 'react-native';
import { Card, CardHeader, CardTitle, CardContent, CardDescription } from '@quanux/ui';
import { Chart } from '@quanux/ui';

export default function HomeScreen() {
    return (
        <ScrollView className="flex-1 bg-background p-4 pt-12">
            <View className="mb-6">
                <Text className="text-3xl font-bold text-foreground">Market Overview</Text>
                <Text className="text-muted-foreground">Welcome back, Duncan.</Text>
            </View>

            <Chart className="mb-6" title="S&P 500" />

            <Text className="mb-4 text-lg font-semibold text-foreground">Top Movers</Text>
            <View className="space-y-4">
                <Card>
                    <CardHeader>
                        <CardTitle>NVIDIA Corp</CardTitle>
                        <CardDescription>NVDA</CardDescription>
                    </CardHeader>
                    <CardContent>
                        <Text className="text-2xl font-bold text-green-500">+4.52%</Text>
                    </CardContent>
                </Card>

                <Card>
                    <CardHeader>
                        <CardTitle>Tesla Inc</CardTitle>
                        <CardDescription>TSLA</CardDescription>
                    </CardHeader>
                    <CardContent>
                        <Text className="text-2xl font-bold text-red-500">-1.20%</Text>
                    </CardContent>
                </Card>
            </View>
        </ScrollView>
    );
}
