# ft_irc — Server Explained

This document explains the purpose and architecture of this IRC-like server implementation, maps key components to relevant RFCs, and gives real-world examples of how the server is used. It is intended to help you understand the codebase and how the pieces fit together.

**Purpose**:
- **What it is**: A minimal IRC-style chat server that accepts client connections, manages channels, routes messages (private and channel), applies basic modes and permissions, and implements core commands.
- **Why it matters**: Provides a lightweight, standards-informed chat service useful for teaching, experimentation, or small-group communication.

**High-level architecture**:
- **Network acceptor / main loop**: The entrypoint sets up sockets, accepts clients, and dispatches I/O handling. See [src/main.cpp](src/main.cpp).
- **Server runtime**: Orchestrates the event loop, timers, and higher-level server lifecycle. See [src/server_runtime.cpp](src/server_runtime.cpp).
- **Connection & client state**: Each connected user is represented and tracked by `client.cpp`/`client.hpp` for registration, nick/user handling, and buffers.
- **Channels & membership**: Channels are managed in `channel.cpp`/`channel.hpp`, which track membership, operators, topics, and modes.
- **Command dispatch**: Individual IRC commands live under `src/commands/` (e.g., `join.cpp`, `privmsg.cpp`, `kick.cpp`). `server.cpp` and `tools.cpp` contain command routing and helpers.

**File-by-file responsibilities** (quick map):
- [src/main.cpp](src/main.cpp): program start, socket initialization, configuration parsing, and launching the runtime.
- [src/server.cpp](src/server.cpp): central server object, global registries (clients, channels), core server utilities and lifecycle helpers.
- [src/server_runtime.cpp](src/server_runtime.cpp): event loop, select/poll handling or other I/O multiplexing, read/write scheduling.
- [src/client.cpp](src/client.cpp): per-connection state machine — registration, nick/user setting, parsing incoming raw lines.
- [src/channel.cpp](src/channel.cpp): channel state, topic handling, join/leave, operator and mode bookkeeping.
- [src/tools.cpp](src/tools.cpp): shared helpers (parsing, formatting replies, validation).
- [src/commands/*.cpp](src/commands/): individual command implementations (JOIN, PRIVMSG, KICK, MODE, TOPIC, INVITE, etc.).

**Common runtime flow (step-by-step)**:
1. Server starts in `main.cpp`, reads configuration and opens a listening socket.
2. `server_runtime` accepts a TCP connection and constructs a `Client` instance.
3. Client sends registration commands (`NICK` and `USER`) — parsed in `client.cpp`.
4. After registration, the client issues `JOIN #channel` handled by `commands/join.cpp`, which updates `Channel` state in `channel.cpp` and notifies members.
5. `PRIVMSG` or `NOTICE` are dispatched by `commands/privmsg.cpp` and routed either to a channel (broadcast to members) or to a target user (direct send).
6. Operator commands like `KICK` or `MODE` check permissions and modify channel/client state.

**Line-level reading approach**:
- The server treats incoming data as CRLF-terminated lines (text protocol). Parsers split lines into tokens: command, parameters, and trailing text. Helpers in `tools.cpp` normalize and validate those tokens before command handlers run.

**RFCs and protocol roles**:
- RFC 1459 (Internet Relay Chat Protocol) — original IRC specification; describes core message formats and commands.
- RFC 2810 (Architecture), RFC 2811 (Channel Modes), RFC 2812 (Client Protocol), RFC 2813 (Server Protocol) — modern clarifications and authoritative guidance on registration, command semantics, numeric replies, and error codes.

How the code maps to RFC responsibilities:
- Registration (`NICK`/`USER`): `client.cpp` enforces the sequencing and replies with numeric responses (see RFC 2812).
- Message format and forwarding (`PRIVMSG`): `commands/privmsg.cpp` implements target parsing and delivery as described in RFC 2812.
- Channel semantics (`JOIN`, `PART`, `TOPIC`, `MODE`): `channel.cpp` and `commands/*` implement modes, topics, and membership rules referenced by RFC 2811/2812.
- Server replies and numerics: `tools.cpp` contains helpers to generate standard numeric replies (e.g., RPL_TOPIC, ERR_NOTREGISTERED) consistent with RFCs.

**Real-life examples / use cases**:
- Team chat for a small project: Multiple developers connect to named channels (project rooms) and coordinate work. `PRIVMSG` is used for both channel discussion and private DMs.
- Public chatroom or community hub: Operators moderate membership and topics; `KICK` and `MODE +b` (ban) are used to control abusive users.
- Bot integration: A bot connects like any client, listens on channels, responds to commands, posts notifications (CI build results, deploy alerts).
- Bridging / gateways: Small gateways can connect this server to other chat systems by acting as clients and relaying messages.

**Security and operational notes**:
- Input validation: Ensure nick/channel names conform to allowed characters and lengths (see helpers in `tools.cpp`).
- Resource limits: Track per-client bandwidth and file descriptors. Use timeouts for registration and idle clients in `server_runtime.cpp`.
- Logging & auditing: Add structured logs when clients register, join channels, or perform operator actions.

**Where to look for specific behavior**:
- Client connection + registration: [src/client.cpp](src/client.cpp)
- Channel joins, state, topic: [src/channel.cpp](src/channel.cpp)
- Command implementations: [src/commands/](src/commands/)
- Server lifecycle: [src/server.cpp](src/server.cpp) and [src/server_runtime.cpp](src/server_runtime.cpp)

**Recommended next steps to deepen understanding**:
1. Read `main.cpp` to see startup flags and sockets.
2. Set breakpoints or add logging in `client.cpp` to observe registration parsing.
3. Run the server locally and connect with a simple IRC client (e.g., `irssi`, `weechat`, or `netcat`) to exercise commands and observe server logs.
4. Compare observed numeric replies with RFC 2812 to confirm compliance.

If you'd like, I can:
- Add inline comments to the top functions in `src/server.cpp` and `src/client.cpp` explaining key blocks line-by-line.
- Create a tutorial that walks through registering a client and joining a channel using `netcat`.

---
Document created to help you explore the codebase and relate implementation to IRC protocol standards and real usage scenarios.
