# QuanuX Client Architecture

The QuanuX client repository is architecturally separated into two distinct ecosystems to ensure stability, performance, and dependency isolation (particularly for styling systems).

## 📂 Directory Structure

### 1. `client/react` (Web & Desktop)
**Target:** Web Browsers, macOS/Windows/Linux Desktops (via Tauri).
- **Core Technology:** React (DOM), Vite.
- **Styling:** **Tailwind CSS v4**.
- **Shared Code:** `client/react/shared` (Components & Logic specific to DOM-based environments).
- **Key Characteristics:**
  - Uses the latest web standards (CSS variables, modern browser APIs).
  - Optimized for mouse/keyboard and extensive data visualization.

### 2. `client/react-native` (Mobile & Devices)
**Target:** iOS, Android, Car, Watch, TV, Vision Pro.
- **Core Technology:** React Native, Expo SDK 52.
- **Styling:** **NativeWind v4** (compatibility mode requiring Tailwind CSS v3).
- **Shared Code:** `client/react-native/ui` (Universal UI components adapted for Native).
- **Key Characteristics:**
  - Strictly typed for Native consistency.
  - Uses `gl-react` or Skia for high-performance mobile graphics.
  - **Critical Dependency Note:** Must strictly adhere to Tailwind v3 for NativeWind compatibility. Do not upgrade to Tailwind v4 until NativeWind v5 is stable.

## 🚀 Development

### Web
```bash
cd client/react/web
pnpm dev
# Runs on http://localhost:5173
```

### Desktop
```bash
cd client/react/desktop/tauri-app
pnpm tauri dev
# Runs on desktop window
```

### Mobile
```bash
cd client/react-native/mobile
pnpm start
# Opens Expo CLI
```

## 🛠 Standards & Skills

Detailed development standards are available for each ecosystem:

- **Web/Desktop Standards:** `client/react/skills/react-frontend-standards/SKILL.md`
  - Focuses on Backend-Driven UI, pure React patterns, and Vercel-style performance optimization.
- **Mobile/Device Standards:** `client/react-native/skills/react-native-standards/SKILL.md`
  - Focuses on Native performance, Expo Router patterns, and NativeWind styling limitations.
