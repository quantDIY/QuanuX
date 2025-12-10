# QuanuX Server

The FastAPI backend for QuanuX.

## Setup

### 1. Install Dependencies
```bash
pip install -r requirements.txt
```

### 2. Configure Secrets
We use the OS Keyring to securely store sensitive configuration.

**Option A: Interactive Setup**
```bash
python -m server.cli.setup_secrets
```

**Option B: Import from .env**
If you have a `.env` file, you can import it directly:
```bash
python -m server.cli.import_env
```

### 3. Refresh Token
To authenticate with Topstep and save a fresh session token to the Keyring:
```bash
python -m server.cli.refresh_token
```

### 4. Run Server
```bash
python -m server.app.main
```
