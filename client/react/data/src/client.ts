import { Client, cacheExchange, fetchExchange, subscriptionExchange, Operation } from 'urql';
import { createClient as createWSClient, Sink } from 'graphql-ws';
import { ExecutionResult, print, ASTNode } from 'graphql';

// Default to localhost for development, but allow override
const DEFAULT_HTTP_URL = 'http://localhost:8000/graphql';
const DEFAULT_WS_URL = 'ws://localhost:8000/graphql';

export interface ClientConfig {
    httpUrl?: string;
    wsUrl?: string;
    token?: string;
}

export function createClient(config: ClientConfig = {}) {
    const httpUrl = config.httpUrl || DEFAULT_HTTP_URL;
    const wsUrl = config.wsUrl || DEFAULT_WS_URL;

    const wsClient = createWSClient({
        url: wsUrl,
        connectionParams: config.token ? { authToken: config.token } : {},
    });

    return new Client({
        url: httpUrl,
        exchanges: [
            cacheExchange,
            fetchExchange,
            subscriptionExchange({
                forwardSubscription: (operation) => ({
                    subscribe: (sink) => {
                        const dispose = wsClient.subscribe(
                            { ...operation, query: print(operation.query as unknown as ASTNode) },
                            sink as Sink<ExecutionResult>
                        );
                        return { unsubscribe: dispose };
                    },
                }),
            }),
        ],
        fetchOptions: () => {
            const headers: Record<string, string> = {};
            if (config.token) {
                headers['Authorization'] = `Bearer ${config.token}`;
            }
            return { headers };
        },
    });
}
