import { Tabs } from 'expo-router';
import { View, Text } from 'react-native';
import '../../global.css';

// Simple icon placeholders until lucide-react-native is confirmed
function TabIcon({ name, color }: { name: string; color: string }) {
    return <Text style={{ color, fontSize: 18 }}>{name === 'index' ? '🏠' : name === 'watchlist' ? '📈' : '💼'}</Text>;
}

export default function Layout() {
    return (
        <Tabs screenOptions={{
            headerShown: false,
            tabBarStyle: { backgroundColor: '#09090b', borderTopColor: '#27272a' },
            tabBarActiveTintColor: '#3b82f6',
            tabBarInactiveTintColor: '#71717a',
        }}>
            <Tabs.Screen
                name="index"
                options={{
                    title: 'Home',
                    tabBarIcon: ({ color }) => <TabIcon name="index" color={color} />,
                }}
            />
            <Tabs.Screen
                name="watchlist"
                options={{
                    title: 'Watchlist',
                    tabBarIcon: ({ color }) => <TabIcon name="watchlist" color={color} />,
                }}
            />
            <Tabs.Screen
                name="portfolio"
                options={{
                    title: 'Portfolio',
                    tabBarIcon: ({ color }) => <TabIcon name="portfolio" color={color} />,
                }}
            />
        </Tabs>
    );
}
