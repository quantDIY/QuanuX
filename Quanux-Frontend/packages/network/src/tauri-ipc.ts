import { createClient, cacheExchange } from 'urql';
// In the production native environment, this utilizes tauri-plugin-graphql-urql IPC
// import { tauriExchange } from 'tauri-plugin-graphql-urql';

const mockedTauriExchange = () => {
    // If running in a browser environment by mistake, this would fallback or error.
    // Assuming tauriExchange is injected here to map directly into the QuanuX Rust backend.
    return require('urql').fetchExchange; 
};

export const tauriGraphQLClient = createClient({
  url: 'ipc://localhost/graphql', // Native local routing 
  exchanges: [
    cacheExchange,
    mockedTauriExchange(), 
  ],
});
