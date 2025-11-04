import React from "react";
import "../index.css";
import { Button } from "../components/ui/button";
import { Card, CardHeader, CardTitle, CardContent } from "../components/ui/card";
import { Health } from "./Health";

export const App: React.FC = () => {
  const [dark, setDark] = React.useState(false);
  React.useEffect(() => {
    document.documentElement.classList.toggle("dark", dark);
  }, [dark]);

  return (
    <div className="min-h-screen p-6">
      <div className="flex items-center justify-between mb-6">
        <h1 className="text-3xl font-bold">QuanuX Web</h1>
        <Button variant="outline" onClick={() => setDark(d => !d)}>
          Toggle {dark ? "Light" : "Dark"}
        </Button>
      </div>

      <Card>
        <CardHeader>
          <CardTitle>Backend / Bridge Health</CardTitle>
        </CardHeader>
        <CardContent>
          <Health />
        </CardContent>
      </Card>
    </div>
  );
};
