import pyotp
import base64
import json
import os
from typing import Optional, Dict, Any, List
from webauthn import (
    generate_registration_options,
    verify_registration_response,
    options_to_json,
    base64url_to_bytes,
    generate_authentication_options,
    verify_authentication_response,
)
from webauthn.helpers.structs import (
    AttestationConveyancePreference,
    AuthenticatorSelectionCriteria,
    AuthenticatorAttachment,
    ResidentKeyRequirement,
    RegistrationCredential,
    AuthenticationCredential,
    UserVerificationRequirement,
)

class JSONCredentialStore:
    """Simple JSON-based credential store for 'out of the box' persistence."""
    def __init__(self, filepath: str = "credentials.json"):
        self.filepath = filepath
        self._load()

    def _load(self):
        if not os.path.exists(self.filepath):
            self.creds = {}
        else:
            try:
                with open(self.filepath, "r") as f:
                    self.creds = json.load(f)
            except Exception:
                self.creds = {}

    def _save(self):
        with open(self.filepath, "w") as f:
            json.dump(self.creds, f, indent=2)

    def save_credential(self, user_id: str, cred_data: Dict[str, Any]):
        if user_id not in self.creds:
            self.creds[user_id] = []
        self.creds[user_id].append(cred_data)
        self._save()

    def get_credentials(self, user_id: str) -> List[Dict[str, Any]]:
        return self.creds.get(user_id, [])

class MFAManager:
    """
    Manages Multi-Factor Authentication (TOTP, WebAuthn).
    """
    
    def __init__(self, app_name: str = "QuanuX", rp_id: str = "localhost", origin: str = "http://localhost:5173"):
        self.app_name = app_name
        self.rp_id = rp_id
        self.origin = origin
        self.store = JSONCredentialStore()

    # --- TOTP (Time-based One-Time Password) ---
    def generate_totp_secret(self) -> str:
        """Generates a random base32 secret for TOTP."""
        return pyotp.random_base32()

    def get_totp_uri(self, secret: str, username: str) -> str:
        """Returns the provisioning URI for QR codes."""
        return pyotp.totp.TOTP(secret).provisioning_uri(
            name=username, 
            issuer_name=self.app_name
        )

    def verify_totp(self, secret: str, code: str) -> bool:
        """Verifies a TOTP code against the secret."""
        totp = pyotp.TOTP(secret)
        return totp.verify(code)

    # --- WebAuthn (Passkeys / YubiKey) ---
    
    def register_webauthn_start(self, user_id: str, username: str) -> str:
        """
        Generates options for WebAuthn registration ceremony.
        Returns JSON string to send to client.
        """
        # Get existing credentials to prevent re-registration
        user_creds = self.store.get_credentials(user_id)
        exclude_credentials = []
        for cred in user_creds:
            exclude_credentials.append({
                "id": base64url_to_bytes(cred["credential_id"]),
                "type": "public-key",
                "transports": cred.get("transports", []),
            })

        options = generate_registration_options(
            rp_name=self.app_name,
            rp_id=self.rp_id,
            user_id=user_id.encode('utf-8'), # Must be bytes
            user_name=username,
            user_display_name=username,
            attestation=AttestationConveyancePreference.NONE,
            authenticator_selection=AuthenticatorSelectionCriteria(
                authenticator_attachment=AuthenticatorAttachment.PLATFORM, # Prefer FaceID/TouchID
                resident_key=ResidentKeyRequirement.PREFERRED,
                user_verification=UserVerificationRequirement.PREFERRED,
            ),
            exclude_credentials=exclude_credentials,
        )
        
        # Store challenge in session (mocked here by returning it, in real app store in DB/Redis)
        # For this stateless implementation, we might need the client to echo it back signed, 
        # or we rely on the fact that verify checks the signature. 
        # BUT `verify_registration_response` NEEDS the original challenge.
        # For now, we will return the options as JSON. The caller (API) needs to manage session state.
        return options_to_json(options)

    def verify_webauthn_registration(self, response_json: str, original_challenge: str, user_id: str) -> bool:
        """
        Verifies the WebAuthn registration response.
        """
        try:
            # Parse response
            # In a real app, we need to parse the JSON from the client
            # The library expects a Request object or dict
            # Let's assume response_json is the body from the client
            
            verification = verify_registration_response(
                credential=RegistrationCredential.parse_raw(response_json),
                expected_challenge=base64url_to_bytes(original_challenge),
                expected_origin=self.origin,
                expected_rp_id=self.rp_id,
            )
            
            # Save credential
            self.store.save_credential(user_id, {
                "credential_id": base64.urlsafe_b64encode(verification.credential_id).decode('utf-8').rstrip('='),
                "public_key": base64.urlsafe_b64encode(verification.credential_public_key).decode('utf-8').rstrip('='),
                "sign_count": verification.sign_count,
                "transports": [], # Can capture from client if provided
            })
            return True
        except Exception as e:
            print(f"WebAuthn Registration Error: {e}")
            return False

    def authenticate_webauthn_start(self, user_id: str) -> str:
        """Generates options for WebAuthn authentication ceremony."""
        user_creds = self.store.get_credentials(user_id)
        allow_credentials = []
        for cred in user_creds:
            allow_credentials.append({
                "id": base64url_to_bytes(cred["credential_id"]),
                "type": "public-key",
            })

        options = generate_authentication_options(
            rp_id=self.rp_id,
            allow_credentials=allow_credentials,
            user_verification=UserVerificationRequirement.PREFERRED,
        )
        return options_to_json(options)

    def verify_webauthn_authentication(self, response_json: str, original_challenge: str, user_id: str) -> bool:
        """Verifies the WebAuthn authentication response."""
        try:
            user_creds = self.store.get_credentials(user_id)
            # We need to find the credential used. The library handles looking up the public key?
            # No, we need to provide the public key for the credential ID in the response.
            
            # Parse first to get ID
            cred = AuthenticationCredential.parse_raw(response_json)
            cred_id_b64 = base64.urlsafe_b64encode(cred.id).decode('utf-8').rstrip('=')
            
            # Find matching cred
            matched_cred = next((c for c in user_creds if c["credential_id"] == cred_id_b64), None)
            if not matched_cred:
                print("Credential not found")
                return False

            verification = verify_authentication_response(
                credential=cred,
                expected_challenge=base64url_to_bytes(original_challenge),
                expected_origin=self.origin,
                expected_rp_id=self.rp_id,
                credential_public_key=base64url_to_bytes(matched_cred["public_key"]),
                credential_current_sign_count=matched_cred["sign_count"],
            )
            
            # Update sign count
            matched_cred["sign_count"] = verification.new_sign_count
            self.store._save() # Persist update
            
            return True
        except Exception as e:
            print(f"WebAuthn Auth Error: {e}")
            return False
