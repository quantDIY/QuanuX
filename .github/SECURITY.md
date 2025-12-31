# Security Policy

## Reporting a Vulnerability

Thank you for taking the time to improve QuanuX.

If you discover a potential security issue, please report it responsibly by opening a **private security issue** in this repository. If GitHub’s interface does not allow private issues for this repository, please contact the project maintainers directly.

Include as much of the following as possible:
- A clear description of the issue
- Steps to reproduce, if applicable
- Any relevant environment or version information
- Suggested mitigation or patches, if available

## Response Expectations

Maintainers will acknowledge the report as soon as practicable. We aim to review and respond in a timely and respectful manner.

## Scope

This policy applies to the QuanuX codebase, including:
- server code
- MCP tooling
- client code
- build and CI workflows

Issues related to ecosystem dependencies (e.g., third-party libraries) should be reported to their respective maintainers.

Thank you for helping keep QuanuX secure and robust.

---

# Security Triage Template

This document describes how security reports for QuanuX are reviewed and handled.

It is intended to provide clarity and consistency, not rigid process.

## 1. Intake

When a security report is received, record:

- **Reporter contact**:
- **Date received**:
- **Affected component(s)**:
  - server
  - client
  - MCP tooling
  - CI / build
  - documentation
- **Reported version / commit**:
- **Summary of issue**:

## 2. Initial Assessment

Perform a quick review to determine:

- Is this a **security issue** (not a functional bug)?
- Is it **reproducible**?
- Is it **in scope** for QuanuX?

If the issue is out of scope (e.g. third-party dependency, configuration misuse), respond with guidance and close.

## 3. Severity Classification

Use the following **lightweight severity levels**:

### 🔴 Critical
- Remote code execution
- Credential leakage
- Authentication or authorization bypass
- Data exfiltration affecting multiple users

**Target response**: Immediate review and mitigation  
**Disclosure**: Coordinated, private until fixed

### 🟠 High
- Privilege escalation
- Sensitive data exposure in common configurations
- Network-exposed attack surfaces with realistic exploit paths

**Target response**: Prompt fix and advisory  
**Disclosure**: Coordinated

### 🟡 Medium
- Information disclosure with limited impact
- Misconfigurations requiring unusual setup
- Exploits requiring significant user interaction

**Target response**: Fix when practical  
**Disclosure**: May be public after fix

### 🟢 Low
- Hard-to-exploit issues
- Defense-in-depth improvements
- Non-sensitive information leaks

**Target response**: Best-effort  
**Disclosure**: May be public

## 4. Mitigation & Resolution

For accepted issues:

- Assign an internal tracking issue or branch
- Implement a fix or mitigation
- Add regression tests where appropriate
- Document the resolution clearly

If the issue is accepted but deferred, note the reason (e.g. pre-release status, architectural change pending).

## 5. Disclosure & Credit

- Reporters will be credited where appropriate, unless anonymity is requested
- Disclosure timing will be coordinated for higher-severity issues
- For pre-release versions, disclosure may be informal or advisory-only

## 6. Post-Mortem (Optional)

For significant issues, briefly document:
- Root cause
- Impact
- Preventive measures

This may be kept internal or summarized publicly, depending on severity.

## Notes

QuanuX is currently in **pre-release**. Security handling reflects that status:
- Best-effort response
- No guaranteed SLA
- Focus on correctness, clarity, and responsible disclosure
