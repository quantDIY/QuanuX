import type { Config } from 'tailwindcss';

const config: Config = {
    content: [
        './src/**/*.{js,jsx,ts,tsx}',
        '../ui/src/**/*.{js,jsx,ts,tsx}', // Allow targets to scan UI package
    ],
    theme: {
        extend: {},
    },
    plugins: [],
};

export default config;
