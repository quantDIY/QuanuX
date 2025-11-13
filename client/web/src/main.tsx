// client/web/src/main.tsx
import "./styles/tailwind.css";  // load v4 CSS here only
import { createRoot } from "react-dom/client";
import { App } from "./ui/App";

createRoot(document.getElementById("root")!).render(<App />);


