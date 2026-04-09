rust-best-practices by apollographql/skills

[Made with love by Vercel](https://vercel.com "Made with love by Vercel")[Skills](/)

[OfficialNew](/official) [Audits](/audits) [Docs](/docs)

[skills](/)/ [apollographql](/apollographql)/ [skills](/apollographql/skills)/rust-best-practices

# rust-best-practices

`$ npx skills add https://github.com/apollographql/skills --skill rust-best-practices`

Summary

**Idiomatic Rust coding standards based on Apollo GraphQL's best practices handbook.**

- Covers nine core areas: coding styles and idioms, clippy linting, performance optimization, error handling, testing patterns, generics and dispatch, type state pattern, documentation, and pointer safety
- Emphasizes borrowing over cloning, Result-based error handling with thiserror/anyhow, and performance profiling with release builds
- Includes quick reference guidance on ownership patterns, panic avoidance, clippy configuration, test naming conventions, and compile-time state safety via type state pattern
- Provides specific lints to enforce (redundant\_clone, large\_enum\_variant, needless\_collect) and recommends `#[expect(...)]` over `#[allow(...)]` with justification comments

SKILL.md

# Rust Best Practices

Apply these guidelines when writing or reviewing Rust code. Based on Apollo GraphQL's [Rust Best Practices Handbook](https://github.com/apollographql/rust-best-practices).

## Best Practices Reference

Before reviewing, familiarize yourself with Apollo's Rust best practices. Read ALL relevant chapters in the same turn in parallel. Reference these files when providing feedback:

- [Chapter 1 - Coding Styles and Idioms](https://github.com/apollographql/skills/blob/HEAD/skills/rust-best-practices/references/chapter_01.md): Borrowing vs cloning, Copy trait, Option/Result handling, iterators, comments
- [Chapter 2 - Clippy and Linting](https://github.com/apollographql/skills/blob/HEAD/skills/rust-best-practices/references/chapter_02.md): Clippy configuration, important lints, workspace lint setup
- [Chapter 3 - Performance Mindset](https://github.com/apollographql/skills/blob/HEAD/skills/rust-best-practices/references/chapter_03.md): Profiling, avoiding redundant clones, stack vs heap, zero-cost abstractions
- [Chapter 4 - Error Handling](https://github.com/apollographql/skills/blob/HEAD/skills/rust-best-practices/references/chapter_04.md): Result vs panic, thiserror vs anyhow, error hierarchies
- [Chapter 5 - Automated Testing](https://github.com/apollographql/skills/blob/HEAD/skills/rust-best-practices/references/chapter_05.md): Test naming, one assertion per test, snapshot testing
- [Chapter 6 - Generics and Dispatch](https://github.com/apollographql/skills/blob/HEAD/skills/rust-best-practices/references/chapter_06.md): Static vs dynamic dispatch, trait objects
- [Chapter 7 - Type State Pattern](https://github.com/apollographql/skills/blob/HEAD/skills/rust-best-practices/references/chapter_07.md): Compile-time state safety, when to use it
- [Chapter 8 - Comments vs Documentation](https://github.com/apollographql/skills/blob/HEAD/skills/rust-best-practices/references/chapter_08.md): When to comment, doc comments, rustdoc
- [Chapter 9 - Understanding Pointers](https://github.com/apollographql/skills/blob/HEAD/skills/rust-best-practices/references/chapter_09.md): Thread safety, Send/Sync, pointer types

## Quick Reference

### Borrowing & Ownership

- Prefer `&T` over `.clone()` unless ownership transfer is required
- Use `&str` over `String`, `&[T]` over `Vec<T>` in function parameters
- Small `Copy` types (≤24 bytes) can be passed by value
- Use `Cow<'_, T>` when ownership is ambiguous

### Error Handling

- Return `Result<T, E>` for fallible operations; avoid `panic!` in production
- Never use `unwrap()`/ `expect()` outside tests
- Use `thiserror` for library errors, `anyhow` for binaries only
- Prefer `?` operator over match chains for error propagation

### Performance

- Always benchmark with `--release` flag
- Run `cargo clippy -- -D clippy::perf` for performance hints
- Avoid cloning in loops; use `.iter()` instead of `.into_iter()` for Copy types
- Prefer iterators over manual loops; avoid intermediate `.collect()` calls

### Linting

Run regularly: `cargo clippy --all-targets --all-features --locked -- -D warnings`

Key lints to watch:

- `redundant_clone` \- unnecessary cloning
- `large_enum_variant` \- oversized variants (consider boxing)
- `needless_collect` \- premature collection

Use `#[expect(clippy::lint)]` over `#[allow(...)]` with justification comment.

### Testing

- Name tests descriptively: `process_should_return_error_when_input_empty()`
- One assertion per test when possible
- Use doc tests ( `///`) for public API examples
- Consider `cargo insta` for snapshot testing generated output

### Generics & Dispatch

- Prefer generics (static dispatch) for performance-critical code
- Use `dyn Trait` only when heterogeneous collections are needed
- Box at API boundaries, not internally

### Type State Pattern

Encode valid states in the type system to catch invalid operations at compile time:

```rust code-highlight
struct Connection<State> { /* ... */ _state: PhantomData<State> }
struct Disconnected;
struct Connected;

impl Connection<Connected> {
    fn send(&self, data: &[u8]) { /* only connected can send */ }
}

```

### Documentation

- `//` comments explain _why_ (safety, workarounds, design rationale)
- `///` doc comments explain _what_ and _how_ for public APIs
- Every `TODO` needs a linked issue: `// TODO(#42): ...`
- Enable `#![deny(missing_docs)]` for libraries

Weekly Installs

5.1K

Repository

[apollographql/skills](https://github.com/apollographql/skills "apollographql/skills")

GitHub Stars

43

First Seen

Jan 26, 2026

Security Audits

[Gen Agent Trust HubPass](/apollographql/skills/rust-best-practices/security/agent-trust-hub) [SocketPass](/apollographql/skills/rust-best-practices/security/socket) [SnykPass](/apollographql/skills/rust-best-practices/security/snyk)

Installed on

claude-code3.8K

opencode3.0K

codex3.0K

gemini-cli3.0K

github-copilot2.9K

kimi-cli2.7K