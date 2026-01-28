import { View, Text, ScrollView } from 'react-native';
import { Card, CardHeader, CardTitle, CardContent, CardDescription, CardFooter } from '@quanux/ui';
import { Button } from '@quanux/ui';
import { Badge } from '@quanux/ui';
import { Avatar, AvatarFallback, AvatarImage } from '@quanux/ui';
import { Input } from '@quanux/ui';
import { Separator } from '@quanux/ui';
import '../../global.css';

export default function DesignShowcase() {
    return (
        <ScrollView className="flex-1 bg-background p-8">
            <View className="mb-8">
                <Text className="text-4xl font-bold text-foreground">QuanuX Design System</Text>
                <Text className="text-muted-foreground">NativeWind v4 + Shadcn Primitives</Text>
            </View>

            {/* Buttons */}
            <View className="mb-8">
                <Text className="text-2xl font-semibold mb-4 text-foreground">Buttons</Text>
                <View className="flex-row flex-wrap gap-4">
                    <Button><Text>Default</Text></Button>
                    <Button variant="secondary"><Text>Secondary</Text></Button>
                    <Button variant="destructive"><Text>Destructive</Text></Button>
                    <Button variant="outline"><Text>Outline</Text></Button>
                    <Button variant="ghost"><Text>Ghost</Text></Button>
                </View>
            </View>

            <Separator className="mb-8" />

            {/* Badges */}
            <View className="mb-8">
                <Text className="text-2xl font-semibold mb-4 text-foreground">Badges</Text>
                <View className="flex-row gap-4">
                    <Badge>Default</Badge>
                    <Badge variant="secondary">Secondary</Badge>
                    <Badge variant="destructive">Destructive</Badge>
                    <Badge variant="outline">Outline</Badge>
                </View>
            </View>

            <Separator className="mb-8" />

            {/* Cards */}
            <View className="mb-8">
                <Text className="text-2xl font-semibold mb-4 text-foreground">Cards</Text>
                <Card className="w-[350px]">
                    <CardHeader>
                        <CardTitle>Create Project</CardTitle>
                        <CardDescription>Deploy your new project in one-click.</CardDescription>
                    </CardHeader>
                    <CardContent>
                        <Text className="text-foreground">Content goes here...</Text>
                    </CardContent>
                    <CardFooter>
                        <Button className="w-full"><Text>Deploy</Text></Button>
                    </CardFooter>
                </Card>
            </View>

            <Separator className="mb-8" />

            {/* Inputs */}
            <View className="mb-8">
                <Text className="text-2xl font-semibold mb-4 text-foreground">Inputs</Text>
                <Input placeholder="Email Address" className="mb-4" />
                <Input placeholder="Password" secureTextEntry />
            </View>
        </ScrollView>
    );
}
