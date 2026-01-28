import { View, Text, ScrollView } from 'react-native';
import { Card, CardHeader, CardTitle, CardContent } from '@quanux/ui';
import { Chart } from '@quanux/ui';
import '../../global.css';

export default function VisionDashboard() {
    return (
        <View className="flex-1 items-center justify-center bg-transparent">
            {/* Main Spatial Window */}
            <Card className="w-[800px] h-[600px] bg-background/80 backdrop-blur-3xl border-white/20 shadow-2xl rounded-[40px]">
                <CardHeader className="flex-row justify-between items-center p-8">
                    <CardTitle className="text-4xl font-bold">QuanuX Spatial</CardTitle>
                    <Text className="text-xl text-muted-foreground">Vision OS</Text>
                </CardHeader>
                <CardContent className="p-8 flex-1">
                    <View className="flex-row h-full gap-8">
                        {/* Left Pane */}
                        <View className="w-1/3 gap-4">
                            <Card className="bg-white/10 border-white/10 p-4">
                                <Text className="text-lg font-semibold">Environment</Text>
                                <Text className="text-sm text-muted">Immersive</Text>
                            </Card>
                            <Card className="bg-white/10 border-white/10 p-4 flex-1">
                                <Text className="text-lg font-semibold mb-2">Active Feeds</Text>
                                <View className="gap-2">
                                    <Text className="text-base p-2 bg-white/5 rounded-lg">Bloomberg TV</Text>
                                    <Text className="text-base p-2 bg-white/5 rounded-lg">CNBC</Text>
                                    <Text className="text-base p-2 bg-white/5 rounded-lg">Twitter AI Stream</Text>
                                </View>
                            </Card>
                        </View>

                        {/* Right Pane (Chart) */}
                        <View className="flex-1">
                            <Chart title="3D Volatility Surface" className="flex-1 bg-white/5 border-white/10" />
                        </View>
                    </View>
                </CardContent>
            </Card>
        </View>
    );
}
