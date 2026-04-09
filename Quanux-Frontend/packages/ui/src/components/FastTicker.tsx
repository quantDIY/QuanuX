import React, { useEffect, useCallback } from 'react';
import { useBeastModeBuffer } from '../hooks/useBeastModeBuffer';
import { useSubscription } from 'urql';

// Mock Hasura GraphQL Subscription querying the live DuckDB pgwire stream
const TICKER_SUBSCRIPTION = `
  subscription OnTickerUpdate($symbol: String!) {
    market_data(where: { symbol: { _eq: $symbol } }, limit: 1, order_by: { timestamp: desc }) {
      symbol
      price
      volume
    }
  }
`;

type TickerData = {
  symbol: string;
  price: number;
  volume: number;
};

export const FastTicker = ({ symbol }: { symbol: string }) => {
  // 1. Initialize the Beast Mode Ref-Buffer.
  // Rendering logic is separated from the React Tree entirely.
  const renderHardwareAcceleratedTicker = useCallback((element: HTMLElement, data: TickerData | null) => {
    if (!data) return;
    
    // Imperative DOM manipulation - zero React reconciliation overhead
    element.textContent = `${data.symbol}: $${data.price.toFixed(2)}`;
    
    // Applying zero-space Tailwind hardware acceleration hints
    element.className = "transform-gpu font-mono text-2xl text-green-500 font-extrabold tracking-tight";
  }, []);

  const { domRef, injectData } = useBeastModeBuffer<TickerData | null>(
    null,
    renderHardwareAcceleratedTicker
  );

  // 2. Subscribe to the Hot Path via the QuanuX Network layer (Hasura WS)
  const [{ data, fetching, error }] = useSubscription({
    query: TICKER_SUBSCRIPTION,
    variables: { symbol },
  });

  // 3. Inject new frames straight into the memory buffer
  useEffect(() => {
    if (data?.market_data?.[0]) {
      injectData(data.market_data[0]);
    }
  }, [data, injectData]);

  if (error) return <div className="text-red-500 p-2 bg-red-900/20 border border-red-500 rounded">Connection Error: {error.message}</div>;

  return (
    <div className="flex flex-col items-center justify-center bg-quanux-dark p-6 rounded-lg shadow-xl border border-gray-800 w-64 h-32">
      {fetching && !data && <span className="text-gray-500 text-sm animate-pulse">Syncing Hasura Supergraph...</span>}
      
      {/* 
        This span is pinned by the Ref-Buffer. 
        React will never re-render this node, guaranteeing 120fps isolation.
      */}
      <span ref={domRef as React.RefObject<HTMLSpanElement>} />
      
      {!fetching && data && (
        <span className="text-xs text-gray-600 mt-2 font-mono uppercase tracking-widest">
          Beast Mode Active
        </span>
      )}
    </div>
  );
};
