# Server Cleanup Notes

## What Was Done
- Split the old monolithic server loop out of `src/server.cpp` into `src/server_runtime.cpp`.
- Kept `src/server.cpp` focused on construction, the line logging hook, and orchestration.
- Moved socket setup, client accept/remove logic, poll handling, and signal shutdown into dedicated helper methods.
- Renamed the typoed password helper to `passwordHasNoWhitespace`.
- Renamed the client buffer helpers to `setValues` and `hasCompleteMessage`.
- Hid the port digit check inside `src/tools.cpp` so it is no longer part of the public server API.

## What Still Needs Work
- Replace the current line logger in `handle_client_line` with real IRC command parsing and dispatch.
- Add dedicated command state handling for registration, authentication, and channel permissions.
- Add tests or scripted smoke checks for connect, disconnect, and malformed input paths.
- Review the remaining utility names in `Clients` and the server for any other typos or protocol-specific cleanup.

## Current Runtime Shape
- `server.cpp` owns startup orchestration.
- `server_runtime.cpp` owns the socket lifecycle and `poll` loop.
- `tools.cpp` only keeps shared parsing helpers.