# IRC Server — Complete TODO & State

## ✅ Done & Working

### Core Runtime
- [x] Server startup: socket → bind → listen
- [x] `SO_REUSEADDR` set on listener socket
- [x] Poll-based event loop (`poll()` with `-1` timeout)
- [x] Signal handling (SIGINT/SIGTERM → `running = 0`)
- [x] Graceful shutdown: loop exits → all sockets closed
- [x] Client connect → `accept()` → pollfd + `Clients` object
- [x] Client disconnect cleanup: socket close, pollfd erase, client erase
- [x] Client disconnect **channel cleanup**: removes member/operator from all channels, deletes empty channels
- [x] 8 KB input buffer limit per client (anti-DoS)
- [x] Line extraction (`\n` delimiter, `\r` stripping)
- [x] Tokenizer (PREFIX / CMD / MIDDLE / TRAILING components)
- [x] Command dispatcher (`handle_commands` routes by CMD string)
- [x] Output buffer flush (`flush_out_buffers` / `send_msg`)
- [x] `process_client_buffers` index-safety guard (checks `i >= client.size()` after command dispatch)

### Command Handlers
- [x] **PASS** — password validation, rejects empty/duplicate
- [x] **NICK** — nickname validation + collision detection
- [x] **USER** — username validation
- [x] **PING/PONG** — echoes payload (e.g. `PONG :hello123`)
- [x] **JOIN** — creates channels, fanout JOIN msg, operator assignment, NAMES list (353/366)
- [x] **PART** — removes from channel, fanout PART msg, deletes empty channels
- [x] **PRIVMSG** — user-to-user and channel fanout, error on missing target

### Public API (command functions moved to `public:` in `server.hpp`)
- [x] `handle_client_line`, `handle_commands`, `process_client_buffers`
- [x] `cl_registration`, `cl_ping`, `cl_join`, `cl_part`, `cl_privmsg`
- [x] `send_msg`, `flush_out_buffers`
- [x] `nickname_coll`, `valid_nick`, `valid_username`

### Test Results: 6/6 PASSED
```
✅ Basic connect + PING/PONG
✅ Registration (PASS+NICK+USER)
✅ JOIN and PART channel
✅ PRIVMSG between users
✅ Registration error handling
✅ Channel fanout (PRIVMSG #channel)
```

---

## 🐛 Bugs to Fix

### 1. No QUIT broadcast when client disconnects
When `recv()` returns 0 (or any error), `remove_client()` is called but no QUIT message is broadcast to channel members.

**Fix**: Before removing client from channels, broadcast a `QUIT` message to all members of each channel the client was in.

**File**: `src/server_runtime.cpp` → `remove_client()`

### 2. Error replies are plain text, not IRC numeric codes
Currently: `"ERR_NOSUCHCHANNEL\r\n"`, `"RPL_WELCOME\r\n"` etc.
Per RFC 2812, should be: `":server 403 <nick> <channel> :No such channel\r\n"`

**Fix**: Replace all error/response strings with proper numeric codes.

**Files**: `src/commands/server_registration.cpp`, `server_join.cpp`, `server_part.cpp`, `server_privmsg.cpp`

### 3. No POLLOUT — blocking `send()` can freeze event loop
`flush_out_buffers()` calls `send()` in a loop. If a slow client's OS buffer fills up, `send()` blocks and the whole server stalls.

**Fix**: Register `POLLOUT` events on fds that have pending `out_buf` data. Write only when poll says writable.

**Files**: `src/server_runtime.cpp` (event loop), `src/commands_tools/server_send.cpp`

### 4. Registration reply should be a proper RFC 2812 welcome
`"RPL_WELCOME\r\n"` should be `":server 001 <nick> :Welcome to the Internet Relay Network <nick>!<user>@<host>\r\n"`

**File**: `src/commands/server_registration.cpp`

### 5. `ERR_ALREADYREGISTRED` typo (missing 'E')
String says `"ERR_ALREADYREGISTRED\r\n"` instead of `"ERR_ALREADYREGISTERED\r\n"`.

**File**: `src/commands/server_registration.cpp`

---

## ⚠️ Medium Priority

### 6. Unregistered clients can call PING
The command dispatcher gates JOIN/PART/PRIVMSG on `client.registred`, but PING bypasses this check.

**Fix**: In `handle_commands`, skip non-registration commands if `!client.registred`.

**File**: `src/commands_tools/server_commands.cpp`

### 7. `passwordHasNoWhitespace` exposed globally
Declared in `server.hpp` → should be in anonymous namespace in `tools.cpp`.

**File**: `src/tools.cpp`

### 8. Hard-coded listen backlog (15)
Not configurable. Should be a named constant.

**File**: `src/server_runtime.cpp`

---

## 📦 Missing Commands to Implement

| Command | Priority | Notes |
|---------|----------|-------|
| QUIT | High | Client-initiated quit + broadcast |
| TOPIC | Medium | Get/set channel topic, permission check |
| MODE | Medium | Channel modes: i, t, k, o, l |
| KICK | Medium | Remove member from channel |
| INVITE | Medium | Invite user to invite-only channel |
| WHOIS | Low | User info lookup |
| NAMES | Low | Standalone NAMES (not just after JOIN) |
| MOTD | Low | Message of the day |
| LIST | Low | List channels |

---

## 🔧 Refactoring & Improvements

### Build & Hygiene
- [ ] Move `passwordHasNoWhitespace` to anonymous namespace in `tools.cpp`
- [ ] Add `#include <cerrno>` explicitly in `server_runtime.cpp` (currently works transitively)
- [ ] Extract `kMaxClientBufferBytes` (8192) as a named constant
- [ ] Extract `kListenBacklog` as a named constant

### Runtime
- [ ] POLLOUT-based non-block writes (see bug #3)
- [ ] `localhost` hostname should be dynamic (`gethostname()`) instead of hard-coded
- [ ] IRC numeric reply constants (e.g. `RPL_WELCOME = "001"`)

### Testing
- [ ] Add test: client disconnect → channel cleanup
- [ ] Add test: PING payload echo
- [ ] Add test: stress test (100+ concurrent clients)
- [ ] Add test: rapid connect/disconnect cycles

---

## 📊 Summary Dashboard

### Runtime (5/5) ✅
| Component | Status |
|-----------|--------|
| Socket setup & listen | ✅ |
| Poll event loop | ✅ |
| Client lifecycle | ✅ |
| Signal handling | ✅ |
| Graceful shutdown | ✅ |

### Commands (7/13) ⏳
| Command | Status |
|---------|--------|
| PASS | ✅ |
| NICK | ✅ |
| USER | ✅ |
| PING/PONG | ✅ (with payload echo) |
| JOIN | ✅ |
| PART | ✅ |
| PRIVMSG | ✅ |
| QUIT | ❌ |
| TOPIC | ❌ |
| MODE | ❌ |
| KICK | ❌ |
| INVITE | ❌ |
| WHOIS | ❌ |

### Quality (4/7) ⏳
| Area | Status |
|------|--------|
| Compiles clean (-Wall -Wextra -Werror) | ✅ |
| All tests pass (6/6) | ✅ |
| Channel cleanup on disconnect | ✅ |
| PING payload echo | ✅ |
| process_client_buffers index safety | ✅ |
| IRC numeric error codes | ❌ |
| POLLOUT non-blocking writes | ❌ |
| QUIT broadcast | ❌ |
| Welcome message per RFC | ❌ |
