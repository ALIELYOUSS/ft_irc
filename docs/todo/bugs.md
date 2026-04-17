# Bugs TODO

## Critical
- [ ] Server initialization is never executed in main flow.
  File: src/main.cpp
  Next step: call server startup path and verify process enters runtime loop.

- [ ] Listening socket file descriptor is not persisted as server state.
  File: src/server.cpp
  Next step: store descriptor as a class member and define ownership lifecycle.

- [ ] Error paths after socket creation do not close the descriptor.
  File: src/server.cpp
  Next step: close descriptor on bind/listen failure before throwing.

- [ ] No connection runtime loop after listen.
  File: src/server.cpp
  Next step: add accept/poll processing loop and graceful shutdown path.

## High
- [ ] Port parsing accepts invalid values silently.
  File: src/server.cpp
  Next step: replace permissive parsing with strict numeric validation and range checks.

- [ ] Main function does not define explicit success return path.
  File: src/main.cpp
  Next step: return explicit status code for success and failures.

- [ ] Socket write crash risk from SIGPIPE behavior.
  File: src/server.cpp
  Next step: define signal handling strategy before client write operations.

## Medium
- [ ] Constructor validation naming is confusing relative to behavior.
  File: src/server.cpp
  Next step: rename predicate to match real intent and reduce misuse risk.

- [ ] Hard-coded listen backlog value is not configurable.
  File: src/server.cpp
  Next step: centralize runtime constants and document chosen defaults.
