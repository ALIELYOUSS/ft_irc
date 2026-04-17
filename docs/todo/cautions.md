# Cautions and Guardrails

## Refactor Cautions
- Keep behavior-preserving changes separate from feature changes.
- Do not mix protocol logic changes with socket lifecycle refactors in one commit.
- Avoid broad renames during functional debugging windows.
- Preserve external command semantics while changing internals.

## Networking Cautions
- Treat every system call as fallible and branch on failures explicitly.
- Ensure descriptor ownership is single and explicit at all times.
- Plan for partial reads/writes; never assume complete transfer in one call.
- Handle client disconnects as normal flow, not exceptional flow.

## Protocol Cautions
- Enforce registration/auth order consistently before channel actions.
- Validate command arity before touching shared state.
- Keep reply formatting centralized to avoid divergent client behavior.
- Guard channel permission checks before mutation actions.

## Testing Cautions
- Verify negative paths first: bad password, bad port, malformed command.
- Repeat tests with two or more concurrent clients for race-like issues.
- Confirm cleanup after forced disconnects and restart loops.
- Track regressions with a written checklist; do not rely on memory.

## Delivery Cautions
- Ship in small milestones with runnable server at each step.
- Update build configuration as new source files become active.
- Avoid declaring command support until numeric replies are correct.
- Document assumptions that are temporary so they are revisited.
