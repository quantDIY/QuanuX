// client/shared/lib/api.ts

const getBaseUrl = () => {
    if (typeof window !== 'undefined' && window.__QUANUX_API_BASE__) {
        return window.__QUANUX_API_BASE__;
    }
    return '/api'; // Default proxy
};

export interface LoginResponse {
    token: string;
    user_id: string;
    scopes: string[];
}

export interface RegisterClientResponse {
    client_id: string;
    client_secret: string;
    message: string;
}

export const api = {
    login: async (username: string, password: string): Promise<LoginResponse> => {
        const res = await fetch(`${getBaseUrl()}/auth/login`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ username, password }),
        });
        if (!res.ok) throw new Error('Login failed');
        return res.json();
    },

    registerClient: async (client_name: string, email?: string): Promise<RegisterClientResponse> => {
        const res = await fetch(`${getBaseUrl()}/auth/register-client`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ client_name, email }),
        });
        if (!res.ok) throw new Error('Registration failed');
        return res.json();
    },

    // Stub for now, can be expanded for GraphQL
    health: async () => {
        const res = await fetch(`${getBaseUrl()}/health/`);
        return res.json();
    }
};
