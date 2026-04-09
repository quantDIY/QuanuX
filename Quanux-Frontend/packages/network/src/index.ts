import React from 'react';
import { Provider } from 'urql';
import { webGraphQLClient } from './pingora-hasura';
import { tauriGraphQLClient } from './tauri-ipc';

export { webGraphQLClient, tauriGraphQLClient };

// Universal wrapper hook that provides generic GraphQL access across Web and Native applications
export const QuanuXNetworkProvider = ({ children, target }: { children: React.ReactNode, target: 'web' | 'tauri' }) => {
  const client = target === 'web' ? webGraphQLClient : tauriGraphQLClient;
  
  return React.createElement(Provider, { value: client }, children);
};
