// SAMPLE ONLY — do not wire until Tauri layer is scaffolded.
// Use a crate like `keyring` for OS keychain.

use serde::Deserialize;

#[derive(Deserialize)]
pub struct KeyArgs {
    key: String,
    value: Option<String>,
}

#[tauri::command]
pub fn keychain_get(key: String) -> Result<Option<String>, String> {
    // TODO: read from OS keychain (macOS Keychain / Windows Credential Manager / libsecret)
    Ok(None)
}

#[tauri::command]
pub fn keychain_set(args: KeyArgs) -> Result<bool, String> {
    // TODO: write to OS keychain
    Ok(false)
}
