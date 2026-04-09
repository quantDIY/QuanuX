import { createClient, cacheExchange, fetchExchange, subscriptionExchange } from 'urql';
import { createClient as createWSClient } from 'graphql-ws';

// Pingora -> Hasura URLs (secrets/URLs from QuanuX Rust Keyring propagated via Vite Env)
const PINGORA_HTTPS_URL = import.meta.env?.VITE_PINGORA_HTTP_URL || 'https://api.quanux.local/v1/graphql';
const PINGORA_WSS_URL = import.meta.env?.VITE_PINGORA_WSS_URL || 'wss://api.quanux.local/v1/graphql';

const wsClient = createWSClient({
  url: PINGORA_WSS_URL,
  // Token authentication headers should be dynamically pushed from Keyring store here
});

export const webGraphQLClient = createClient({
  url: PINGORA_HTTPS_URL,
  exchanges: [
    cacheExchange,
    fetchExchange,
    subscriptionExchange({
      forwardSubscription(request) {
        const input = { ...request, query: request.query || '' };
        return {
          subscribe(sink) {
            const unsubscribe = wsClient.subscribe(input, sink);
            return { unsubscribe };
          },
        };
      },
    }),
  ],
});
