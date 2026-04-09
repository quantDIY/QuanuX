import i18n from 'i18next';
import { initReactI18next } from 'react-i18next';
import LocizeBackend from 'i18next-locize-backend';

// These secrets are pulled from Rust Keyring in production according to QuanuX standards.
// For dev placeholder, we use fallback values.
const locizeOptions = {
  projectId: process.env.VITE_LOCIZE_PROJECTID || 'quanux_locize_placeholder_id',
  apiKey: process.env.VITE_LOCIZE_APIKEY || 'quanux_locize_placeholder_key', 
  version: process.env.VITE_LOCIZE_VERSION || 'latest'
};

i18n
  .use(LocizeBackend)
  .use(initReactI18next)
  .init({
    fallbackLng: 'en',
    debug: false,
    backend: locizeOptions,
    interpolation: {
      escapeValue: false, // React strictly escapes by default
    }
  });

export default i18n;
