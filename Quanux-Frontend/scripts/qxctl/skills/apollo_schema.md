graphql-schema by apollographql/skills

[Made with love by Vercel](https://vercel.com "Made with love by Vercel")[Skills](/)

[OfficialNew](/official) [Audits](/audits) [Docs](/docs)

[skills](/)/ [apollographql](/apollographql)/ [skills](/apollographql/skills)/graphql-schema

# graphql-schema

`$ npx skills add https://github.com/apollographql/skills --skill graphql-schema`

Summary

**Industry best practices guide for designing intuitive, performant, and maintainable GraphQL schemas.**

- Covers core design principles including client-centric type organization, explicit nullability patterns, and backwards-compatible evolution strategies
- Provides reference documentation on types, naming conventions, cursor-based pagination, error modeling, and security considerations
- Includes practical patterns for interfaces, unions, input types, mutations, and ID strategies with code examples
- Emphasizes ground rules for descriptions, non-null fields, list patterns, and avoiding breaking changes through deprecation

SKILL.md

# GraphQL Schema Design Guide

This guide covers best practices for designing GraphQL schemas that are intuitive, performant, and maintainable. Schema design is primarily a server-side concern that directly impacts API usability.

## Schema Design Principles

### 1\. Design for Client Needs

- Think about what queries clients will write
- Organize types around use cases, not database tables
- Expose capabilities, not implementation details

### 2\. Be Explicit

- Use clear, descriptive names
- Make nullability intentional
- Document with descriptions

### 3\. Design for Evolution

- Plan for backwards compatibility
- Use deprecation before removal
- Avoid breaking changes

## Quick Reference

### Type Definition Syntax

```graphql code-highlight
"""
A user in the system.
"""
type User {
  id: ID!
  email: String!
  name: String
  posts(first: Int = 10, after: String): PostConnection!
  createdAt: DateTime!
}

```

### Nullability Rules

PatternMeaningStringNullable - may be nullString!Non-null - always has value\[String\]Nullable list, nullable items\[String!\]Nullable list, non-null items\[String\]!Non-null list, nullable items\[String!\]!Non-null list, non-null items

**Best Practice:** Use **\[Type!\]!** for lists - empty list over null, no null items.

### Input vs Output Types

```graphql code-highlight
# Output type - what clients receive
type User {
  id: ID!
  email: String!
  createdAt: DateTime!
}

# Input type - what clients send
input CreateUserInput {
  email: String!
  name: String
}

# Mutation using input type
type Mutation {
  createUser(input: CreateUserInput!): User!
}

```

### Interface Pattern

```graphql code-highlight
interface Node {
  id: ID!
}

type User implements Node {
  id: ID!
  email: String!
}

type Post implements Node {
  id: ID!
  title: String!
}

```

### Union Pattern

```graphql code-highlight
union SearchResult = User | Post | Comment

type Query {
  search(query: String!): [SearchResult!]!
}

```

## Reference Files

Detailed documentation for specific topics:

- [Types](https://github.com/apollographql/skills/blob/HEAD/skills/graphql-schema/references/types.md) \- Type design patterns, interfaces, unions, and custom scalars
- [Naming](https://github.com/apollographql/skills/blob/HEAD/skills/graphql-schema/references/naming.md) \- Naming conventions for types, fields, and arguments
- [Pagination](https://github.com/apollographql/skills/blob/HEAD/skills/graphql-schema/references/pagination.md) \- Connection pattern and cursor-based pagination
- [Errors](https://github.com/apollographql/skills/blob/HEAD/skills/graphql-schema/references/errors.md) \- Error modeling and result types
- [Security](https://github.com/apollographql/skills/blob/HEAD/skills/graphql-schema/references/security.md) \- Security best practices for schema design

## Key Rules

### Type Design

- Define types based on domain concepts, not data storage
- Use interfaces for shared fields across types
- Use unions for mutually exclusive types
- Keep types focused (single responsibility)
- Avoid deep nesting - flatten when possible

### Field Design

- Fields should be named from client's perspective
- Return the most specific type possible
- Make expensive fields explicit (consider arguments)
- Use arguments for filtering, sorting, pagination

### Mutation Design

- Use single input argument pattern: `mutation(input: InputType!)`
- Return affected objects in mutation responses
- Model mutations around business operations, not CRUD
- Consider returning a union of success/error types

### ID Strategy

- Use globally unique IDs when possible
- Implement `Node` interface for refetchability
- Base64-encode compound IDs if needed

## Ground Rules

- ALWAYS add descriptions to types and fields
- ALWAYS use non-null ( **!**) for fields that cannot be null
- ALWAYS use **\[Type!\]!** pattern for lists
- NEVER expose database internals in schema
- NEVER break backwards compatibility without deprecation
- PREFER dedicated input types over many arguments
- PREFER enums over arbitrary strings for fixed values
- USE `ID` type for identifiers, not `String` or `Int`
- USE custom scalars for domain-specific values (DateTime, Email, URL)

Weekly Installs

1.1K

Repository

[apollographql/skills](https://github.com/apollographql/skills "apollographql/skills")

GitHub Stars

43

First Seen

Jan 23, 2026

Security Audits

[Gen Agent Trust HubPass](/apollographql/skills/graphql-schema/security/agent-trust-hub) [SocketPass](/apollographql/skills/graphql-schema/security/socket) [SnykPass](/apollographql/skills/graphql-schema/security/snyk)

Installed on

opencode892

codex883

github-copilot865

gemini-cli864

claude-code837

cursor797