import './global.css';
import { StatusBar } from 'expo-status-bar';
import { View, Text } from 'react-native';
import { Button } from '@quanux/ui';
import { cn } from '@quanux/ui';

export default function App() {
  return (
    <View className="flex-1 items-center justify-center bg-background p-4">
      <Text className="text-2xl font-bold mb-4 text-foreground">QuanuX Mobile</Text>

      <Button onPress={() => console.log('Parsed!')}>
        <Text className="text-primary-foreground">Action Button</Text>
      </Button>

      <StatusBar style="auto" />
    </View>
  );
}
