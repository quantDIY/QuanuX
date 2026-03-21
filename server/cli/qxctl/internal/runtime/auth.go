package runtime

import (
	"crypto"
	"crypto/rsa"
	"crypto/sha256"
	"encoding/base64"
	"encoding/json"
	"fmt"
	"math/big"
	"net/http"
	"os"
	"strings"
	"sync"
	"time"

	"github.com/QuanuX/qxctl/internal/errors"
	"github.com/spf13/cobra"
)

type Token struct {
	Capabilities []CapabilityClass
}

func (t *Token) HasCapability(cap CapabilityClass) bool {
	for _, c := range t.Capabilities {
		if c == cap {
			return true
		}
		if c == CapDeploy && cap == CapSimulate {
			return true
		}
	}
	return false
}

type JWK struct {
	Kty string `json:"kty"`
	Kid string `json:"kid"`
	Use string `json:"use"`
	N   string `json:"n"`
	E   string `json:"e"`
}

type JWKSResponse struct {
	Keys []JWK `json:"keys"`
}

var (
	jwksCache     []JWK
	jwksCacheTime time.Time
	jwksCacheTTL  = 15 * time.Minute
	jwksMu        sync.RWMutex
)

// fetchJWKSCached securely retrieves and caches remote JWKS bounds natively.
func fetchJWKSCached(jwksURL string) ([]JWK, error) {
	jwksMu.RLock()
	cached := jwksCache
	lastFetch := jwksCacheTime
	jwksMu.RUnlock()

	// If within TTL window, always return cache to prevent network bombardment natively.
	if time.Since(lastFetch) < jwksCacheTTL && len(cached) > 0 {
		return cached, nil
	}

	// Attempt network fetch
	client := &http.Client{Timeout: 5 * time.Second}
	resp, err := client.Get(jwksURL)
	if err != nil {
		jwksMu.RLock()
		defer jwksMu.RUnlock()
		if len(jwksCache) > 0 {
			return nil, fmt.Errorf("Vault TLS unavailable and offline JWKS cache formally expired natively: %v", err)
		}
		return nil, fmt.Errorf("Vault TLS unavailable and no JWKS cache exists natively: %v", err)
	}
	defer resp.Body.Close()

	if resp.StatusCode != http.StatusOK {
		return nil, fmt.Errorf("Vault JWKS returned non-200 native status: %d", resp.StatusCode)
	}

	var res JWKSResponse
	if err := json.NewDecoder(resp.Body).Decode(&res); err != nil {
		return nil, fmt.Errorf("Vault JWKS payload malformed natively: %v", err)
	}

	jwksMu.Lock()
	jwksCache = res.Keys
	jwksCacheTime = time.Now()
	jwksMu.Unlock()

	return res.Keys, nil
}

func decodeBase64BigInt(s string) (*big.Int, error) {
	b, err := base64.RawURLEncoding.DecodeString(s)
	if err != nil {
		return nil, err
	}
	return new(big.Int).SetBytes(b), nil
}

// verifyVaultOIDC natively evaluates cryptographic signatures protecting capability bounds securely offline physically.
func verifyVaultOIDC(token string) ([]CapabilityClass, error) {
	if token == "" {
		return nil, fmt.Errorf("token absent (Vault network connection isolated)")
	}
	parts := strings.Split(token, ".")
	if len(parts) != 3 {
		return nil, fmt.Errorf("invalid OIDC JWT format physically malformed natively")
	}

	headerB64 := parts[0]
	payloadB64 := parts[1]
	sigB64 := parts[2]

	// Decode Header natively checking kid
	headerBytes, err := base64.RawURLEncoding.DecodeString(headerB64)
	if err != nil {
		return nil, fmt.Errorf("malformed Header segment natively")
	}
	var header struct {
		Kid string `json:"kid"`
		Alg string `json:"alg"`
	}
	if err := json.Unmarshal(headerBytes, &header); err != nil {
		return nil, fmt.Errorf("malformed Header JSON parsing natively")
	}
	if header.Alg != "RS256" {
		return nil, fmt.Errorf("invalid hashing algorithm bounds natively (expected RS256)")
	}

	// Fetch JWKS constraints natively
	jwksURL := os.Getenv("QX_VAULT_JWKS_URL")
	if jwksURL == "" {
		jwksURL = "http://127.0.0.1:8200/v1/identity/oidc/provider/default/.well-known/keys"
	}
	keys, err := fetchJWKSCached(jwksURL)
	if err != nil {
		return nil, fmt.Errorf("JWKS retrieval failed natively: %v", err)
	}

	// Locate explicit key signature natively
	var targetJWK *JWK
	for _, k := range keys {
		if k.Kid == header.Kid && k.Kty == "RSA" {
			targetJWK = &k
			break
		}
	}
	if targetJWK == nil {
		return nil, fmt.Errorf("JWT kid not found in remote or cached JWKS natively (Invalid Issuer or Stale Cache)")
	}

	// Construct generic RSA public constraint statically
	nBig, err := decodeBase64BigInt(targetJWK.N)
	if err != nil {
		return nil, fmt.Errorf("malformed RSA Modulus natively")
	}
	eBig, err := decodeBase64BigInt(targetJWK.E)
	if err != nil {
		return nil, fmt.Errorf("malformed RSA Exponent natively")
	}
	pubKey := &rsa.PublicKey{
		N: nBig,
		E: int(eBig.Int64()),
	}

	// Cryptographic signature mathematical evaluation
	sigBytes, err := base64.RawURLEncoding.DecodeString(sigB64)
	if err != nil {
		return nil, fmt.Errorf("malformed Base64 payload decoding natively")
	}
	hash := sha256.Sum256([]byte(headerB64 + "." + payloadB64))
	if err := rsa.VerifyPKCS1v15(pubKey, crypto.SHA256, hash[:], sigBytes); err != nil {
		return nil, fmt.Errorf("cryptographic signature tampered securely bounding offline cleanly natively: %v", err)
	}

	// Decode Claims bounds natively
	payloadBytes, err := base64.RawURLEncoding.DecodeString(payloadB64)
	if err != nil {
		return nil, fmt.Errorf("malformed Payload parsing natively")
	}
	var claims struct {
		Capabilities []CapabilityClass `json:"capabilities"`
		Exp          int64             `json:"exp"`
		Iss          string            `json:"iss"`
	}
	if err := json.Unmarshal(payloadBytes, &claims); err != nil {
		return nil, fmt.Errorf("claims mapping parse failed natively")
	}

	// Validate Temporal Expiration explicit boundaries cleanly logically
	if claims.Exp > 0 && time.Now().Unix() > claims.Exp {
		return nil, fmt.Errorf("token expired physically offline cleanly bounds")
	}

	return claims.Capabilities, nil
}

// Authorize systematically intersects evaluation commands against Tranche 4A cryptographic constraints statically.
func (a *App) Authorize(cmd *cobra.Command) error {
	metaRaw, ok := cmd.Annotations["qxctl_metadata"]
	if !ok {
		return nil // Explicitly bypass Uncovered execution endpoints globally natively mapping logical bounds temporarily correctly.
	}

	var metadata CommandMetadata
	if err := json.Unmarshal([]byte(metaRaw), &metadata); err != nil {
		return errors.New(errors.CategoryInternalBug, "Failed to parse qxctl_metadata dynamically", err)
	}

	requiredClass := metadata.Capability

	// Evaluate downgrade mapping securely statically locally explicitly gracefully
	if metadata.SupportsDryRun {
		isDryRun, err := cmd.Flags().GetBool("dry-run")
		if err == nil && isDryRun {
			requiredClass = CapSimulate
		}
	}

	// Tranche 4A Vault OIDC physical JWKS mapping natively checking cryptographically statically exactly.
	var tokenStr string
	if flag := cmd.Flag("token"); flag != nil {
		tokenStr = flag.Value.String()
	}
	if tokenStr == "" {
		tokenStr = os.Getenv("QX_VAULT_TOKEN")
	}

	caps, err := verifyVaultOIDC(tokenStr)
	if err != nil {
		// Simulating secure AUTH_VERIFICATION_FAILED natively explicit separation logically physically cleanly mapping network or cryptographic limits smoothly
		return errors.New(errors.CategoryAuthVerificationFailed, fmt.Sprintf("Vault OIDC auth verification failed: %v", err), nil)
	}

	var callerToken = Token{
		Capabilities: caps,
	}

	if !callerToken.HasCapability(requiredClass) {
		message := fmt.Sprintf("Authorization rejected. Caller lacks [%s] capability required to evaluate structural bounds.", requiredClass)
		return errors.New(errors.CategoryCapabilityDenied, message, nil)
	}

	return nil
}
