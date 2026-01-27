---
name: topstepx-extension
description: Instructions for using and maintaining the TopstepX Extension.
---

# TopstepX Extension Skill

The **TopstepX Extension** (`quanux-topstepx`) provides the primary interface for trading and account management with TopstepX.

## Location
- **Path**: `extensions/python/topstepx`
- **Source**: `extensions/python/topstepx/src`
- **Tests**: `extensions/python/topstepx/tests`

## Capabilities
1.  **Authentication**: Automatic login flow using credentials from the system keyring.
2.  **Order Management**: Place, Modify, and Cancel orders.
3.  **Position Management**: View open positions, Close (flatten), and Partial Close.
4.  **Market History**: Retrieve historical price bars.

## Usage in Agent Tasks

When asked to "modify Topstep functions" or "debug Topstep connection":

1.  **Check Configuration**:
    Use `quanuxctl topstepx env` (`run_command`) to view current settings.
    *Do not ask user for password/api key in chat unless absolutely necessary for re-entry.*

2.  **Run Tests**:
    The test suite is the **primary verification method**.
    ```bash
    pytest extensions/python/topstepx/tests
    ```
    This runs against the **LIVE** API using the user's keychain credentials.

3.  **Source Code Navigation**:
    - `src/auth.py`: Logic for obtaining `Bearer` tokens.
    - `src/orders.py`: REST endpoints for `/api/Order/*`.
    - `src/positions.py`: REST endpoints for `/api/Position/*`.

## Environment Variables
The extension automatically loads these from the environment or Keyring. You typically do **not** need to set them manually in scripts.

- `QUANUX_TOPSTEP__USERNAME`
- `QUANUX_TOPSTEP__PASSWORD`
- `QUANUX_TOPSTEP__API_KEY`
- `QUANUX_TOPSTEP__BASE_API_URL` (Default: `https://api.topstepx.com`)

## Adding New Endpoints
If the user requests a new API endpoint (e.g. `Account/Reset`):
1.  Locate the appropriate file in `src/` (or create a new one like `src/account_actions.py`).
2.  Import `BaseModel` from `models.py` for request/response typing.
3.  Use `httpx` with the `token` passed from the caller.
4.  Add a corresponding test file in `tests/`.

## Common Issues
- **401 Unauthorized**: Usually means the API Key is invalid or expired. Run `quanuxctl topstepx apikey <new_key>`.
- **404 Not Found**: Check `QUANUX_TOPSTEP__BASE_API_URL`. Ensure it matches the environment (Production vs Sim).
