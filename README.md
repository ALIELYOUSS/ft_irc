*This project has been created as part of the 42 curriculum by yael-maa, alel-you.*

# ft_irc — Internet Relay Chat Server

## Description

**ft_irc** is an IRC (Internet Relay Chat) server implemented from scratch in **C++98**. The goal of the project is to build a server that speaks the IRC protocol well enough to be used with a real IRC client (such as irssi, WeeChat, or HexChat), allowing several users to connect simultaneously, authenticate, exchange private messages, and join group channels.

The project demonstrates how the Internet works at the protocol level: IRC is a text-based protocol over TCP/IP, governed by public standards (RFC 1459 and its successors). Building a server from scratch means parsing raw messages, managing sockets in a non-blocking way, and correctly handling concurrent connections — all without any external library.

This is the mandatory part of the 42 school **ft_irc** project. Only the server is implemented: no IRC client and no server-to-server communication.

---

## Features

- **Multi-client support** — the server handles many clients simultaneously without hanging, using a **single `poll()`** event loop (non-blocking file descriptors, no forking).
- **Registration flow** — clients authenticate with `PASS`, set a nickname with `NICK`, and a username with `USER` before they can interact with the network.
- **Channels** — users can `JOIN` and `PART` channels, receive the topic and the member list, and exchange messages.
- **Public and private messaging** — `PRIVMSG` forwards channel messages to every other member of the channel and delivers direct messages to a specific user.
- **Operators** — each channel has operators and regular users. The first user joining a channel automatically becomes its operator.
- **Operator commands**:
  - `KICK` — eject a client from a channel
  - `INVITE` — invite a client to a channel
  - `TOPIC` — change or view the channel topic
  - `MODE` — change the channel's mode with the following flags:
    - `i` — set/remove invite-only channel
    - `t` — set/remove the restriction of the `TOPIC` command to channel operators
    - `k` — set/remove the channel key (password)
    - `o` — give/take channel operator privilege
    - `l` — set/remove the user limit to the channel
- **Robust data handling** — incoming packets are aggregated to rebuild complete commands (partial data / low bandwidth safe), and input/output buffers are bounded to protect the server.
- **Graceful shutdown** — `SIGINT`/`SIGTERM` handlers stop the event loop cleanly and close every socket.

---

## Instructions

### Requirements

- A C++ compiler supporting **C++98** (the project is compiled with `c++`).
- **GNU Make**.
- A **reference IRC client** to test the server (e.g. irssi, WeeChat, HexChat, or `nc` for a quick test).
- Only standard C++98 and POSIX system calls are used — **no external libraries, no Boost**.

### Compilation

Clone the repository and run `make`:

```sh
make
```

The `Makefile` provides the standard rules `all`, `clean`, `fclean` and `re`, and compiles with the flags `-Wall -Wextra -Werror -std=c++98`:

```sh
make        # build the ircserv binary
make clean  # remove object files
make fclean # remove object files and the binary
make re     # full rebuild
```

### Execution

Run the server with a listening port and a connection password:

```sh
./ircserv <port> <password>
```

- **port** — the port number the server listens on (must be between 1024 and 65535).
- **password** — the connection password required by any client that connects.

### Connecting a client

Connect your IRC client (e.g. irssi) to the server:

```sh
/connect <server-host> <port> <password>
```

### Quick test with `nc`

The server correctly aggregates partial data. Sending a command in several chunks with `nc` (`-C` for CRLF) must still work:

```sh
$ nc -C 127.0.0.1 6667
PASS secret
NICK bob
USER bob 0 * :Bob User
```

Use `Ctrl+D` to send a command in several parts (e.g. `com`, then `man`, then `d`): the server buffers the packets and rebuilds the full command before processing it.

---

## Usage examples

### Registration

A client must first register before using the network:

```text
PASS secret
NICK bob
USER bob 0 * :Bob
```

The server replies with a welcome message (`001`) once all three are provided.

### Joining a channel

```text
JOIN #general
```

The first user to join automatically becomes the channel operator (`@`). The server replies with the topic (`331`/`332`), the member list (`353`) and the end of the names list (`366`).

### Sending a private message

```text
PRIVMSG #general :Hello everyone!
PRIVMSG alice :Hi Alice!
```

### Operator commands

```text
MODE #general +o alice     # give operator privilege to alice
MODE #general +i           # make the channel invite-only
MODE #general +k secret    # set a channel key
MODE #general +l 10        # limit the channel to 10 users
KICK #general alice :bye   # eject alice from the channel
INVITE alice #general      # invite alice to the channel
TOPIC #general :New topic  # change the channel topic
```

---

## Technical choices and architecture

- **Language and standard** — the entire codebase complies with the **C++98** standard and is compiled with `-Wall -Wextra -Werror`.
- **I/O model** — a single `poll()` loop monitors the listening socket and every client socket. All file descriptors are set to **non-blocking** mode with `fcntl(fd, F_SETFL, O_NONBLOCK)`. `recv()`/`send()` are never called outside the readiness provided by `poll()`. This keeps the server responsive with many clients while using minimal resources.
- **Message parsing** — incoming lines are tokenized into IRC components (prefix, command, middle parameters, trailing parameter), following the IRC message grammar.
- **Buffers** — per-client input buffer (8 KB) and output buffer (100 KB) prevent memory exhaustion; partial messages are accumulated until a complete CRLF-terminated line is received.
- **Data structures** — the server keeps a registry of clients, a map of channels, and per-channel member/operator/invited sets (implemented with `std::map` and `std::set`).
- **Project layout**:

```text
.
├── Makefile
├── includes/
│   ├── server.hpp      # Server class declaration
│   ├── clients.hpp     # Client class declaration
│   └── channel.hpp     # Channel class declaration
└── src/
    ├── main.cpp             # entry point, argument validation
    ├── server.cpp           # constructor, initialization
    ├── server_runtime.cpp   # poll() loop, accept/recv/send, signal handling
    ├── client.cpp           # buffering and IRC message tokenization
    ├── channel.cpp          # channel state and membership logic
    ├── tools.cpp            # port parsing, helpers
    ├── commands_tools/      # command dispatch, buffering, sending
    └── commands/            # one file per command (PASS, NICK, USER, JOIN,
                             # PART, PRIVMSG, TOPIC, KICK, INVITE, MODE, PING)
```

---

## Resources

### Classic references

- **RFC 1459** — *Internet Relay Chat Protocol* — the original IRC specification.
- **RFC 2810** — *Internet Relay Chat: Architecture*.
- **RFC 2811** — *Internet Relay Chat: Channel Management*.
- **RFC 2812** — *Internet Relay Chat: Client Protocol*.
- **RFC 2813** — *Internet Relay Chat: Server Protocol*.
- **Modern IRC documentation** — <https://modern.ircdocs.horse/> — a practical, up-to-date reference for IRC message format, numerics and commands.
- **irssi** — <https://irssi.org/> — a popular text-based IRC client, used as the reference client for testing.
- **Beej's Guide to Network Programming** — <https://beej.us/guide/bgnet/> — a classic introduction to sockets and network programming in C.
- **42 School subject** 
