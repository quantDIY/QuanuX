import { StatusBar } from 'expo-status-bar';
import { StyledView, StyledText } from '@quanux/ui';

export default function App() {
  return (
    <StyledView className="flex-1 items-center justify-center bg-black">
      <StyledText className="text-5xl font-bold text-white">Welcome to QuanuX TV</StyledText>
      <StatusBar style="light" />
    </StyledView>
  );
}
