import { View, Text, TouchableOpacity } from 'react-native';
import { Card } from '@quanux/ui';
import '../../global.css';

// Car UI typically requires large touch targets and distinct colors.
export default function CarDashboard() {
    return (
        <View className="flex-1 bg-black p-6 flex-row flex-wrap gap-6 justify-center items-center">
            {/* Top Row */}
            <TouchableOpacity className="w-[45%] aspect-square">
                <Card className="flex-1 bg-blue-900 border-blue-700 items-center justify-center p-4">
                    <Text className="text-6xl mb-2">📈</Text>
                    <Text className="text-2xl font-bold text-white">Markets</Text>
                    <Text className="text-xl text-green-400 mt-2">+1.2%</Text>
                </Card>
            </TouchableOpacity>

            <TouchableOpacity className="w-[45%] aspect-square">
                <Card className="flex-1 bg-zinc-900 border-zinc-700 items-center justify-center p-4">
                    <Text className="text-6xl mb-2">💰</Text>
                    <Text className="text-2xl font-bold text-white">Portfolio</Text>
                    <Text className="text-xl text-white mt-2">$24.5K</Text>
                </Card>
            </TouchableOpacity>

            {/* Bottom Row */}
            <TouchableOpacity className="w-[45%] aspect-square">
                <Card className="flex-1 bg-zinc-900 border-zinc-700 items-center justify-center p-4">
                    <Text className="text-6xl mb-2">🎙️</Text>
                    <Text className="text-2xl font-bold text-white">News</Text>
                </Card>
            </TouchableOpacity>

            <TouchableOpacity className="w-[45%] aspect-square">
                <Card className="flex-1 bg-red-900 border-red-700 items-center justify-center p-4">
                    <Text className="text-6xl mb-2">⚠️</Text>
                    <Text className="text-2xl font-bold text-white">Alerts</Text>
                    <Text className="text-xl text-white mt-2">2 New</Text>
                </Card>
            </TouchableOpacity>
        </View>
    );
}
