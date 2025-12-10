import { StatusBar } from 'expo-status-bar';
import { StyledView, StyledText } from '@quanux/ui';

export default function App() {
  return (
    <StyledView className="flex-1 items-center justify-center bg-transparent">
      <StyledText className="text-6xl font-bold text-white glass">Welcome to QuanuX Vision</StyledText>
      <StatusBar style="light" />
    </StyledView>
  );
}
