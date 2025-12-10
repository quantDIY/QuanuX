import { StatusBar } from 'expo-status-bar';
import { StyledView, StyledText } from '@quanux/ui';

export default function App() {
  return (
    <StyledView className="flex-1 items-center justify-center bg-white">
      <StyledText className="text-3xl font-bold">Welcome to QuanuX Tablet</StyledText>
      <StatusBar style="auto" />
    </StyledView>
  );
}
