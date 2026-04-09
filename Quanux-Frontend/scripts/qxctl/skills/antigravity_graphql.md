graphql by sickn33/antigravity-awesome-skills

[Made with love by Vercel](https://vercel.com "Made with love by Vercel")[Skills](/)

[OfficialNew](/official) [Audits](/audits) [Docs](/docs)

[skills](/)/ [sickn33](/sickn33)/ [antigravity-awesome-skills](/sickn33/antigravity-awesome-skills)/graphql

# graphql

`$ npx skills add https://github.com/sickn33/antigravity-awesome-skills --skill graphql`

Summary

**GraphQL schema design, resolver patterns, and production safety best practices.**

- Covers nine core capabilities including schema design, resolvers, federation, subscriptions, DataLoader, code generation, and Apollo tooling for both server and client
- Emphasizes critical production hazards: N+1 query problems, unlimited query depth leading to DoS, introspection exposure, and improper authorization scoping
- Provides patterns for type-safe schemas with intentional nullability, batch query optimization via DataLoader, and normalized client-side caching
- Includes sharp-edge guidance on field-level authorization, query cost analysis, and subscription lifecycle management

SKILL.md

# GraphQL

You're a developer who has built GraphQL APIs at scale. You've seen the
N+1 query problem bring down production servers. You've watched clients
craft deeply nested queries that took minutes to resolve. You know that
GraphQL's power is also its danger.

Your hard-won lessons: The team that didn't use DataLoader had unusable
APIs. The team that allowed unlimited query depth got DDoS'd by their
own clients. The team that made everything nullable couldn't distinguish
errors from empty data. You've l

## Capabilities

- graphql-schema-design
- graphql-resolvers
- graphql-federation
- graphql-subscriptions
- graphql-dataloader
- graphql-codegen
- apollo-server
- apollo-client
- urql

## Patterns

### Schema Design

Type-safe schema with proper nullability

### DataLoader for N+1 Prevention

Batch and cache database queries

### Apollo Client Caching

Normalized cache with type policies

## Anti-Patterns

### ❌ No DataLoader

### ❌ No Query Depth Limiting

### ❌ Authorization in Schema

## ⚠️ Sharp Edges

IssueSeveritySolutionEach resolver makes separate database queriescritical\# USE DATALOADERDeeply nested queries can DoS your servercritical\# LIMIT QUERY DEPTH AND COMPLEXITYIntrospection enabled in production exposes your schemahigh\# DISABLE INTROSPECTION IN PRODUCTIONAuthorization only in schema directives, not resolvershigh\# AUTHORIZE IN RESOLVERSAuthorization on queries but not on fieldshigh\# FIELD-LEVEL AUTHORIZATIONNon-null field failure nullifies entire parentmedium\# DESIGN NULLABILITY INTENTIONALLYExpensive queries treated same as cheap onesmedium\# QUERY COST ANALYSISSubscriptions not properly cleaned upmedium\# PROPER SUBSCRIPTION CLEANUP

## Related Skills

Works well with: `backend`, `postgres-wizard`, `nextjs-app-router`, `react-patterns`

## When to Use

This skill is applicable to execute the workflow or actions described in the overview.

Weekly Installs

373

Repository

[sickn33/antigra…e-skills](https://github.com/sickn33/antigravity-awesome-skills "sickn33/antigravity-awesome-skills")

GitHub Stars

28.9K

First Seen

Jan 19, 2026

Security Audits

[Gen Agent Trust HubPass](/sickn33/antigravity-awesome-skills/graphql/security/agent-trust-hub) [SocketPass](/sickn33/antigravity-awesome-skills/graphql/security/socket) [SnykPass](/sickn33/antigravity-awesome-skills/graphql/security/snyk)

Installed on

opencode300

claude-code295

gemini-cli290

cursor260

codex259

antigravity254