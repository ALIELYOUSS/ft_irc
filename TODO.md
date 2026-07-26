# Fix Plan - Server Not Sending Messages

## Bugs Fixed

| # | Bug | File | Fix |
|---|-----|------|-----|
| 1 | `appendMsg(msg, size)` appended from position `size` (end of string) — **all data silently discarded** | `src/client.cpp` | Changed `this->buffer.append(msg, size)` → `this->buffer.append(msg, 0, size)` |
| 2 | `PASS` command required NICK/USER to be set first (inverted registration flow) | `src/commands_tools/server_registration.cpp` | Removed the `NICK/USER first` check |
| 3 | `USER` command expected exactly 1 param (`!= cmpnt_index + 2`), but IRC USER has 4 params | `src/commands_tools/server_registration.cpp` | Changed `!=` → `<` |
| 4 | `handle_commands()` iterated ALL clients instead of just the one that sent data | `src/commands_tools/server_commands.cpp` | Changed to accept `client_index`, removed outer loop |
| 5 | Header signature didn't match | `includes/server.hpp` | Updated `handle_commands()` → `handle_commands(size_t client_index)` |
| 6 | `server_buffer.cpp` called `handle_commands()` without index | `src/commands_tools/server_buffer.cpp` | Changed `handle_commands()` → `handle_commands(i)` |

## Testing Results

- **PING → PONG**: ✅ Works (output: `PONG`)
- **Registration (PASS+NICK+USER)**: ✅ Works (output: `RPL_WELCOME`)
- **Build**: ✅ `make re` — 0 errors, 0 warnings
