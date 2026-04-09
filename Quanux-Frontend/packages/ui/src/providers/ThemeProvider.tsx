import React, { createContext, useContext, useEffect, useRef } from "react";

type Theme = "dark" | "light" | "system";
type Direction = "ltr" | "rtl";

type ThemeProviderProps = {
  children: React.ReactNode;
  defaultTheme?: Theme;
  defaultDirection?: Direction;
  storageKey?: string;
};

type ThemeProviderState = {
  theme: Theme;
  setTheme: (theme: Theme) => void;
  direction: Direction;
  setDirection: (direction: Direction) => void;
};

const initialState: ThemeProviderState = {
  theme: "system",
  setTheme: () => null,
  direction: "ltr",
  setDirection: () => null,
};

const ThemeProviderContext = createContext<ThemeProviderState>(initialState);

export function ThemeProvider({
  children,
  defaultTheme = "system",
  defaultDirection = "ltr",
  storageKey = "quanux-ui-theme",
  ...props
}: ThemeProviderProps) {
  // Enforce Basic Tenets strictly: Bypassing useState guarantees 0 Virtual DOM Layout Thrashing 
  // globally through the React Context Tree. The DOM is mutated synchronously via References.
  const themeRef = useRef<Theme>(defaultTheme);
  const dirRef = useRef<Direction>(defaultDirection);

  useEffect(() => {
    if (typeof window !== "undefined") {
      themeRef.current = (localStorage.getItem(storageKey) as Theme) || defaultTheme;
      dirRef.current = (localStorage.getItem(`${storageKey}-dir`) as Direction) || defaultDirection;
      applyThemeToDom(themeRef.current);
      applyDirToDom(dirRef.current);
    }
  }, [defaultTheme, defaultDirection, storageKey]);

  const applyThemeToDom = (theme: Theme) => {
    const root = window.document.documentElement;
    root.classList.remove("light", "dark");
    if (theme === "system") {
      const systemTheme = window.matchMedia("(prefers-color-scheme: dark)").matches ? "dark" : "light";
      root.classList.add(systemTheme);
    } else {
      root.classList.add(theme);
    }
  };

  const applyDirToDom = (dir: Direction) => {
    window.document.documentElement.setAttribute("dir", dir);
  };

  // We provide imperative mutators instead of reactive state setters.
  // Child components dynamically read CSS roots directly rather than re-rendering natively.
  const value = {
    theme: themeRef.current,
    setTheme: (theme: Theme) => {
      localStorage.setItem(storageKey, theme);
      themeRef.current = theme;
      applyThemeToDom(theme);
    },
    direction: dirRef.current,
    setDirection: (direction: Direction) => {
      localStorage.setItem(`${storageKey}-dir`, direction);
      dirRef.current = direction;
      applyDirToDom(direction);
    }
  };

  return (
    <ThemeProviderContext.Provider {...props} value={value}>
      {children}
    </ThemeProviderContext.Provider>
  );
}

export const useTheme = () => {
  const context = useContext(ThemeProviderContext);
  if (context === undefined)
    throw new Error("useTheme must be used within a ThemeProvider");
  return context;
};
