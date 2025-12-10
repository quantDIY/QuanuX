import { StatusBar } from 'expo-status-bar';
import { StyledView, StyledText } from '@quanux/ui';

export default function App() {
  return (
    <StyledView className="flex-1 items-center justify-center bg-black">
      <StyledText className="text-sm font-bold text-white text-center">Welcome to QuanuX Wear</StyledText>
      <StatusBar style="light" />
    </StyledView>
  );
}
