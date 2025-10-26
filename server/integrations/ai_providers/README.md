# QuanuX AI Provider Registry (Scaffold) — v0.0.1

This directory registers external and local AI providers and specifies their
capabilities, suggested base URLs, auth modes, and rate-limiting metadata.
**No code** and **no secrets** live here—only manifests and setup notes.

## Design goals
- Single place to list providers and their capabilities
- Zero-secrets: keys/tokens are retrieved via the Secrets Broker at runtime
- Stable shape for agents and adapters to introspect
- Easy to add new providers by copying `_templates/provider.manifest.yaml`

## Files
- `registry.yaml` — master list of enabled/known providers
- `<provider>/manifest.yaml` — provider-specific settings (no secrets)
- `<provider>/README.md` — where to get credentials + usage notes
- `_templates/provider.manifest.yaml` — canonical schema for new providers

## How auth works (at wiring time)
- The server fetches tokens/keys from the Secrets Broker (OS keychain/Vault/Redis-cache)
- Env var names are **documented** here but values are not stored in git
- Providers with OAuth2/OpenID will use server-side exchanges and short-lived tokens

See also:
- `/server/security/secrets.py` for the broker façade
- `/docs/AI/manifest.yaml` for platform-level AI defaults (coming later)
