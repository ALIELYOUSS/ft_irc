# Bugs TODO

## Critical
- [x] No graceful shutdown path from runtime loop.
  File: src/server.cpp
  Next step: add a stop condition (signal flag or command), break the loop, then close all fds.

- [x] Client input buffer can grow without limit (memory DoS risk).
  File: src/client.cpp
  Next step: enforce a max buffered size per client and disconnect or truncate when exceeded.

## High
- [x] Port parsing accepts invalid values silently.
  File: src/server.cpp
  Next step: replace `atoi` with strict parsing (`strtol`) and reject trailing garbage/overflow.

- [x] `main` has no explicit return codes for success/failure paths.
  File: src/main.cpp
  Next step: return explicit status codes (`0` on success, non-zero on errors).

- [x] No socket reuse option on listener (restart can fail with address in use).
  File: src/server.cpp
  Next step: set `SO_REUSEADDR` before bind and validate return value.

- [ ] Channel join/membership rules are implemented in model but not wired into runtime command path.
  File: src/server.cpp
  Next step: parse JOIN commands in `handle_client_line` and mutate `server::channels` using `Channel` APIs.

## Medium
- [x] Helper naming is confusing (`is_wspace` validates password whitespace policy).
  File: src/server.cpp
  Next step: rename helper to reflect behavior, e.g. `passwordHasNoWhitespace`.

- [ ] Hard-coded listen backlog value is not configurable.
  File: src/server.cpp
  Next step: centralize runtime constants and document chosen defaults.

- [x] `close_socks` takes fd vector by value, causing unnecessary copy.
  File: includes/server.hpp
  Next step: pass by const reference or close owned `server::fds` directly.

- [x] Global function `isValidChannelName` is exported at translation-unit scope.
  File: src/channel.cpp
  Next step: mark it `static` or place it in an anonymous namespace to avoid symbol collisions.

- [ ] `passwordHasNoWhitespace` is still non-static at translation-unit scope.
  File: src/server.cpp
  Next step: make helper `static` (or move into anonymous namespace) to avoid global symbol leakage.

- [x] `errno.h` include is no longer needed after removing `strtol` path.
  File: src/server.cpp
  Next step: remove unused include and keep includes minimal.
