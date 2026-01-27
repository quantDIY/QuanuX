---
name: react-native-standards
description: Expert guidance on QuanuX mobile development, enforcing NativeWind v4 usage, Expo Router patterns, and high-performance native lists, distinct from the web ecosystem.
metadata:
  version: 1.0.0
  author: QuanuX
  priority: 10
---

# QuanuX Mobile & Device Standards (React Native)

You are an expert QuanuX Mobile Developer specializing in React Native, Expo, and NativeWind.
Your primary responsibility is to build performant, native-feeling applications for iOS, Android, and specialized devices (Car, Watch, TV) while maintaining architectural consistency with the QuanuX Backend-Driven model.

## 1. Architectural Protocols (CRITICAL)

### [RULE 1] Backend Origin
**Same as Web:** All data MUST be generated on the backend.
- Mobile clients are strictly presentation layers.
- **Do not** embed complex business logic in the app binary. This allows over-the-air (OTA) updates to logic via backend changes without app store submission.

### [RULE 2] Platform Awareness
**The UI must adapt to the platform context.**
- Use `Platform.OS` or `Platform.select({})` to fork behavior for iOS/Android/Web.
- Respect safe areas (notches, dynamic islands) using `react-native-safe-area-context`.

## 2. Tech Stack & Compatibility (STRICT)

The mobile ecosystem has strict version requirements distinct from the web.

- **Framework**: Expo SDK 52 (React Native 0.76).
- **Styling**: **NativeWind v4** (Requires Tailwind CSS v3 Compatibility).
  - **WARNING**: Do NOT upgrade to Tailwind v4 until NativeWind v5 is fully stable and approved.
  - Use `className` prop for styling.
  - Use `gl-react` or `react-native-skia` for high-performance graphics, *not* HTML Canvas.
- **Navigation**: Expo Router (file-based routing similar to Next.js).

## 3. React Native Best Practices

### A. List Performance (Critical)
- **Problem**: Mobile devices have limited memory and CPU compared to desktops.
- **Solution**: ALWAYS use `FlashList` (from `@shopify/flash-list`) instead of `FlatList` for long lists.
- **Do not** render massive lists of components without virtualization.

### B. Animations
- **Use `react-native-reanimated`** for 60/120fps UI thread animations.
- **Avoid** `Animated` from react-native core for complex gestures.
- **Avoid** JS-driven animations that block the bridge.

### C. Bundle Size & OTA
- Keep the JS bundle small to ensure fast Over-The-Air updates.
- Lazy load heavy screens or features not immediately required on launch.

## 4. UI Library Protocol

- **Location**: Shared UI components live in `client/react-native/ui`.
- **Usage**: Import from `@quanux/ui`.
- **Pattern**: When creating a new primitive, stick to the `shadcn` pattern:
  1. Define primitive in `ui/src/components/ui/[name].tsx`.
  2. Export via `ui/src/index.ts`.
  3. Ensure it works with NativeWind class names.

## 5. Directory Structure
- `app/`: Expo Router pages.
- `components/`: Feature-specific components.
- `assets/`: Images/Fonts.
- `constants/`: App-wide constants (colors, config).
