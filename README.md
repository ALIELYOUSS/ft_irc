# ft_irc – IRC Server Implementation

A C++98 IRC (Internet Relay Chat) server implementation using poll-based I/O, supporting multiple concurrent clients, channels, and core IRC commands.

---

## Quick Start

### Build
```bash
make
```

### Run
```bash
./ircserv <port> <password>
```

**Example:**
```bash
./ircserv 6667 secretpass
```

### Test
Connect with netcat or any IRC client:
```bash
nc localhost 6667
```

---

## Project Overview

This is a single-threaded, event-driven IRC server built with:
- **Poll-based I/O** for high concurrency (~1000+ clients)
- **No threads** – synchronous handling via poll(2)
- **C++98** – compatible with older compilers
- **Clean modular design** – separated runtime, commands, and models

### Features (In Progress)
- ✅ Socket setup and client lifecycle
- ✅ Signal handling (graceful shutdown)
- ✅ Input buffering and line extraction
- ⏳ Command parsing and dispatch
- ⏳ Client registration (PASS/NICK/USER)
- ⏳ Channels and membership
- ⏳ Core IRC commands (JOIN, PRIVMSG, TOPIC, MODE, KICK, INVITE)
- ⏳ Output queuing and partial writes

---

## Project Structure

```
ft_irc/
├── includes/
│   ├── server.hpp       # Server orchestration interface
│   ├── clients.hpp      # Client state model
│   ├── channel.hpp      # Channel model and membership rules
│   └── command.hpp      # Command registration (stub)
├── src/
│   ├── main.cpp              # Entry point
│   ├── server.cpp            # Orchestration and logging
│   ├── server_runtime.cpp    # Socket, poll loop, I/O
│   ├── client.cpp            # Client state implementation
│   ├── channel.cpp           # Channel implementation
│   ├── tools.cpp             # Parsing helpers
│   └── commands/
│       ├── auth.cpp          # PASS/NICK/USER (stubs)
│       ├── join.cpp          # JOIN (stub)
│       ├── privmsg.cpp       # PRIVMSG (stub)
│       ├── topic.cpp         # TOPIC (stub)
│       ├── mode.cpp          # MODE (stub)
│       ├── kick.cpp          # KICK (stub)
│       └── invite.cpp        # INVITE (stub)
├── docs/
│   └── todo/
│       ├── next-steps.md     # Development roadmap
│       ├── bugs.md           # Known issues
│       ├── cautions.md       # Design guardrails
│       └── server-cleanup.md # Refactor notes
├── Makefile
└── README.md
```

---

## Architecture

### Runtime Flow

1. **Startup** (`main.cpp` → `server.cpp`)
   - Parse port and password
   - Construct server
   - Call `init()` to start the runtime

2. **Socket Setup** (`server_runtime.cpp::setup_listener()`)
   - Create TCP socket, set `SO_REUSEADDR`, bind, listen
   - Register listener in poll array

3. **Event Loop** (`server_runtime.cpp::run_event_loop()`)
   - Poll for activity on listener and all client sockets
   - Accept new connections
   - Receive data, parse lines, extract complete commands
   - Currently logs lines to stdout; next phase will dispatch to handlers

4. **Shutdown** (on SIGINT/SIGTERM)
   - Signal handler sets `running = 0`
   - Event loop exits
   - All sockets closed, cleanup done

### Key Components

| Module | Purpose |
|--------|---------|
| `server.cpp` | Thin orchestration layer; `init()` calls setup, signals, loop |
| `server_runtime.cpp` | Poll loop, socket I/O, client lifecycle |
| `client.cpp` | Circular buffer for input; line extraction |
| `channel.cpp` | Channel model with members, operators, invite-only rules |
| `tools.cpp` | Parsing helpers: port validation, password checks |
| `commands/*` | Command handlers (to be implemented) |

---

## Building and Configuration

### Compiler Flags
```makefile
-Wall -Wextra -Werror -std=c++98
```

Strict compilation; no warnings allowed.

### Dependencies
- Standard C library (socket, poll, signal)
- C++98 STL (vector, map, set, string, sstream)

### Build Targets
```bash
make        # Build ircserv
make clean  # Remove object files
make fclean # Remove objects and binary
make re     # Clean + build
```

---

## Usage

### Running the Server
```bash
./ircserv 6667 mypassword
```

- **Port**: Must be 1024–65535 (no privileged ports)
- **Password**: No whitespace allowed; sent by clients via PASS command

### Connecting with netcat
```bash
nc localhost 6667
```

Type any text; it will be logged by the server as:
```
[fd 4] HELLO WORLD
```

### Connecting with IRC Client
Any standard IRC client (irssi, Hexchat, etc.):
- Host: `localhost`
- Port: `6667`
- Password: Your chosen server password

---

## Development Roadmap

See [docs/todo/next-steps.md](docs/todo/next-steps.md) for detailed phases.

### Phase 1: Stabilize Core Runtime ✅
- Socket setup, listen, accept
- Poll-based event loop
- Signal handling
- Client lifecycle (connect, disconnect, cleanup)

### Phase 2: Build Connection Model ✅
- Client state and buffer management
- Channel model with membership rules
- Line-based input parsing

### Phase 3: Command Pipeline ⏳ *Next*
- Tokenizer and command dispatcher
- Registration state machine (PASS → NICK → USER)
- Authentication gates

### Phase 4: Feature Delivery ⏳
- JOIN, PART, PRIVMSG
- TOPIC, MODE, KICK, INVITE
- Channel permissions and fanout

### Phase 5: Validation & Hardening ⏳
- Test suite and smoke checks
- Edge case handling
- Observability and logging

---

## Known Issues & Limitations

See [docs/todo/bugs.md](docs/todo/bugs.md) for full list.

- **No command parsing yet** – Lines are logged but not dispatched
- **No output queue** – Replies would block on slow clients
- **No registration state** – All clients can attempt any command
- **Hard-coded backlog** – Listen backlog is 15; not configurable
- **Buffer limit is 8 KB per client** – Enforced to prevent memory exhaustion

---

## Design Decisions

### Thread Model
- **Single-threaded, poll-based I/O**
- No mutexes, no thread spawning
- Scales to ~1000 concurrent clients
- Simpler to reason about; easier to debug

### Error Handling
- Exceptions propagate to `main()` for cleanup
- All system calls checked; no silent failures
- Descriptors owned by server; no shared pointers

### Ownership
- Each client owns its fd, buffer, state
- Each channel owns its members, operators, invite list
- Server owns all clients, channels, fds
- No aliasing or shared ownership

### Performance
- Poll array rebuilt on every connect/disconnect (acceptable for up to 1000 clients)
- Circular buffer for input; no reallocations during line extraction
- Stateless command dispatch (no session trees)

---

## Testing

### Manual Testing
```bash
# Terminal 1: Start server
./ircserv 6667 testpass

# Terminal 2: Connect with netcat
nc localhost 6667

# Type commands (currently logged as-is)
HELLO
JOIN #channel
PRIVMSG #channel :Hello world
```

### Recommended Next Steps
1. Build test suite using `irctest` or similar
2. Verify PASS/NICK/USER registration sequence
3. Test JOIN and channel membership
4. Stress test: 100+ concurrent connects, rapid disconnects
5. Verify cleanup: no leaks, all fds closed

---

## Code Quality

- **Compilation**: Clean with `-Wall -Wextra -Werror`
- **Style**: Consistent naming, clear separation of concerns
- **Documentation**: Inline comments on complex logic; see docs/todo/ for design notes
- **Modular design**: Easy to add new commands without touching core runtime

---

## Contributing & Next Steps

### For Command Implementation
1. Implement tokenizer in `handle_client_line()`
2. Add handler stubs in `src/commands/<command>.cpp`
3. Register in command dispatcher
4. Test with IRC client

### For Feature Development
- See [docs/todo/next-steps.md](docs/todo/next-steps.md) for phases
- See [REFACTOR_SUMMARY.md](REFACTOR_SUMMARY.md) for current runtime shape

### For Bug Fixes
- See [docs/todo/bugs.md](docs/todo/bugs.md) for priority list

---

## References

- [RFC 2812 – IRC Protocol](https://tools.ietf.org/html/rfc2812)
- [poll(2) man page](https://man7.org/linux/man-pages/man2/poll.2.html)
- [socket(2) man page](https://man7.org/linux/man-pages/man2/socket.2.html)

---

## License

This project is part of a coding challenge (42 school). See project guidelines for licensing.

---

## Quick Reference: IRC Commands (Stubs)

| Command | Status | Purpose |
|---------|--------|---------|
| PASS | ⏳ | Server authentication |
| NICK | ⏳ | Set nickname |
| USER | ⏳ | Set username/realname |
| JOIN | ⏳ | Join a channel |
| PART | ⏳ | Leave a channel |
| PRIVMSG | ⏳ | Send message |
| TOPIC | ⏳ | Set channel topic |
| MODE | ⏳ | Set user/channel modes |
| KICK | ⏳ | Remove user from channel |
| INVITE | ⏳ | Invite user to channel |

All are implemented as stubs; awaiting dispatcher wiring.

---

## Troubleshooting

### "Address already in use" on startup
- Server was recently killed; `SO_REUSEADDR` is set but OS still holds port for 30–60s
- Wait a minute or use a different port

### "poll failed" error
- Check file descriptor limits: `ulimit -n`
- May need to increase if running many clients

### Server exits immediately with no error
- Check that port is in range [1024, 65535]
- Check that password has no spaces or tabs

### Clients can't connect
- Ensure firewall allows the port
- Verify server is listening: `netstat -tlnp | grep 6667`

---

## Summary

This is a **clean, modular IRC server** with:
- ✅ Solid runtime foundation (socket, poll, signals, cleanup)
- ✅ Well-designed data models (clients, channels)
- ⏳ Command infrastructure ready for implementation
- 📚 Clear documentation and roadmap

**Next milestone**: Implement command dispatcher and PASS/NICK/USER registration.

