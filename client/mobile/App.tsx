import React from "react";
import { SafeAreaView, Text, View, Button } from "react-native";

export default function App() {
  const [pinged, setPinged] = React.useState<string>("(not yet)");
  const healthUrl = "http://10.0.2.2:8077/health"; // Android emulator -> host

  return (
    <SafeAreaView>
      <View style={{ padding: 16 }}>
        <Text style={{ fontSize: 22, fontWeight: "600" }}>QuanuX Mobile</Text>
        <Text>Backend/bridge status: {pinged}</Text>
        <Button title="Ping bridge" onPress={async () => {
          try {
            const r = await fetch(healthUrl);
            setPinged(r.ok ? "OK" : `HTTP ${r.status}`);
          } catch (e) {
            setPinged("error");
          }
        }} />
      </View>
    </SafeAreaView>
  );
}
