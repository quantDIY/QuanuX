import { useEffect, useRef } from 'react';

/**
 * Beast Mode Ref-Buffer Hook
 * 
 * Bypasses the React Virtual DOM entirely for high-throughput data streams (e.g. L2 Order Books).
 * It buffers incoming payloads into a useRef and paints the DOM imperatively inside a continuous
 * requestAnimationFrame loop, allowing exactly 120fps hardware-accelerated updates 
 * without triggering React component reconciliations or layout thrashing.
 */
export function useBeastModeBuffer<T>(
  initialValue: T,
  renderFn: (element: HTMLElement, data: T) => void
) {
  const domRef = useRef<HTMLElement | null>(null);
  const dataRef = useRef<T>(initialValue);
  const dirtyRef = useRef<boolean>(true);
  const rafRef = useRef<number | null>(null);

  // Bypasses React state. The payload is instantly accepted into memory.
  const injectData = (newData: T) => {
    dataRef.current = newData;
    dirtyRef.current = true;
  };

  useEffect(() => {
    const loop = () => {
      if (dirtyRef.current && domRef.current) {
        // Direct imperatively-mapped DOM mutation
        renderFn(domRef.current, dataRef.current);
        dirtyRef.current = false;
      }
      rafRef.current = requestAnimationFrame(loop);
    };

    rafRef.current = requestAnimationFrame(loop);
    return () => {
      if (rafRef.current) cancelAnimationFrame(rafRef.current);
    };
  }, [renderFn]);

  return { domRef, injectData };
}
