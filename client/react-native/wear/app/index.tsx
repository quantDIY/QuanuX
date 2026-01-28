import { View, Text, ScrollView } from 'react-native';
import { Card, CardHeader, CardTitle, CardContent } from '@quanux/ui';
import '../../global.css';

export default function WearDashboard() {
    return (
        <ScrollView className="flex-1 bg-black p-2">
            <Card className="mb-2 bg-zinc-900 border-zinc-800">
                <CardHeader className="p-2">
                    <CardTitle className="text-xs text-muted-foreground">Portfolio</CardTitle>
                </CardHeader>
                <CardContent className="p-2 pt-0">
                    <Text className="text-xl font-bold text-white">$24,592</Text>
                    <Text className="text-xs text-green-500">+1.9%</Text>
                </CardContent>
            </Card>

            <Text className="text-xs text-muted-foreground mb-1 ml-1">Watchlist</Text>

            <Card className="mb-1 bg-zinc-900 border-zinc-800 p-2 flex-row justify-between items-center">
                <View>
                    <Text className="text-sm font-bold text-white">NVDA</Text>
                </View>
                <View>
                    <Text className="text-sm text-green-500">+4.5%</Text>
                </View>
            </Card>

            <Card className="mb-1 bg-zinc-900 border-zinc-800 p-2 flex-row justify-between items-center">
                <View>
                    <Text className="text-sm font-bold text-white">TSLA</Text>
                </View>
                <View>
                    <Text className="text-sm text-red-500">-1.2%</Text>
                </View>
            </Card>
        </ScrollView>
    );
}
