# 🧭 QuanuX Operations Cheat Sheet

> Unified local operations guide for NATS, control agent, and bridge orchestration  
> **Location:** `server/docs/ops_cheatsheet.md`

---

## ⚙️ Daily Use

```bash
# 1️⃣ Activate venv and move to project root
source ~/.venvs/quanux/bin/activate
cd ~/Projects/QuanuX/quanux-full-context-v1

# 2️⃣ Start the control plane agent (Terminal 1)
export PYTHONPATH="$PWD:$PYTHONPATH"
python -m server.control.nats_agent

# 3️⃣ Start NATS server (Terminal 2)
nats-server -DV

# 4️⃣ Manage the bridge (Terminal 3)
qxctl bridge up --name signalr --runtime flask --port 8077
qxctl bridge status --name signalr
qxctl bridge list
qxctl bridge down --name signalr

# 5️⃣ Verify HTTP health
curl -s http://127.0.0.1:8077/health | jq .
```

---

## 🧩 When Port Already in Use

```bash
# Check PID stored by the control agent
cat server/control/state/signalr/pid || echo "no pid"

# Kill stale process if needed
kill -TERM $(cat server/control/state/signalr/pid)

# Remove old state
rm -rf server/control/state/signalr

# Relaunch bridge cleanly
qxctl bridge up --name signalr --runtime flask --port 8077
```

---

## 🧰 Reinstall / Repair CLI

```bash
# From repo root
source ~/.venvs/quanux/bin/activate
python -m pip install -e server/cli

# Confirm CLI is in venv
which qxctl
qxctl --help
```

If you see `"scaffold not available yet"`, reload modules:
```bash
python - <<'PY'
import importlib, sys
mods = [m for m in sys.modules if m and m.startswith('qxctl')]
for m in mods: sys.modules.pop(m, None)
import qxctl, inspect
print('qxctl main at:', inspect.getsourcefile(qxctl))
PY
```

---

## 🧾 Aliases (Add to `~/.zshrc`)

```bash
alias qx-venv='source ~/.venvs/quanux/bin/activate'
alias qx-root='cd ~/Projects/QuanuX/quanux-full-context-v1'
alias qx-agent='qx-root && export PYTHONPATH="$PWD:$PYTHONPATH" && python -m server.control.nats_agent'
alias qx-nats='nats-server -DV'
alias qx-up='qxctl bridge up --name signalr --runtime flask --port 8077'
alias qx-st='qxctl bridge status --name signalr'
alias qx-down='qxctl bridge down --name signalr'
```

Then you can launch the full stack:

```bash
qx-venv
qx-root
# Three terminals:
qx-nats
qx-agent
qx-up && qx-st
```

---

## 🪄 Optional: Autostart via systemd (User-Level)

### NATS Server
```bash
mkdir -p ~/.config/systemd/user
cat > ~/.config/systemd/user/nats-server.service <<'UNIT'
[Unit]
Description=NATS Server (user)
After=network.target

[Service]
ExecStart=%h/go/bin/nats-server -DV
Restart=on-failure

[Install]
WantedBy=default.target
UNIT

systemctl --user daemon-reload
systemctl --user enable --now nats-server.service
```

### Control Agent
```bash
cat > ~/.config/systemd/user/quanux-nats-agent.service <<'UNIT'
[Unit]
Description=QuanuX Control Agent
After=network.target nats-server.service

[Service]
WorkingDirectory=%h/Projects/QuanuX/quanux-full-context-v1
Environment=PYTHONPATH=%h/Projects/QuanuX/quanux-full-context-v1
Environment=PATH=%h/.venvs/quanux/bin:%h/.local/bin:/usr/bin
ExecStart=%h/.venvs/quanux/bin/python -m server.control.nats_agent
Restart=on-failure

[Install]
WantedBy=default.target
UNIT

systemctl --user daemon-reload
systemctl --user enable --now quanux-nats-agent.service
```

Then, the control stack runs automatically at login — you can just:
```bash
qxctl bridge up --name signalr --runtime flask --port 8077
```

---

## 🧹 Reset / Cleanup

```bash
# Remove stale state
rm -rf server/control/state/signalr

# Reinstall CLI if broken
source ~/.venvs/quanux/bin/activate
python -m pip install -e server/cli

# Verify all paths
which qxctl
qxctl --help
```

---

✅ **You’re operational.**  
Once this file is saved to `server/docs/ops_cheatsheet.md`, you can view it beautifully rendered in GitHub or VS Code, and copy-paste directly from Markdown preview.
