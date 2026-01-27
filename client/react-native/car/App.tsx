import { StatusBar } from 'expo-status-bar';
import { StyledView, StyledText } from '@quanux/ui';

export default function App() {
  return (
    <StyledView className="flex-1 items-center justify-center bg-zinc-900">
      <StyledText className="text-4xl font-bold text-white">Welcome to QuanuX Car</StyledText>
      <StatusBar style="light" />
    </StyledView>
  );
}
