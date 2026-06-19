# ft_irc Testing & Debugging Guide

## ✅ Server Status

Your IRC server now has **comprehensive debugging output** integrated throughout the runtime. All major operations are logged in real-time.

---

## 🚀 Quick Start Testing

### Terminal 1: Start Server
```bash
cd /home/alel-you/ft_irc
./ircserv 6667 testpassword
```

**Expected output:**
```
[INIT] Server constructed. Port: 6667, Password: testpassword
[INIT] Starting server initialization...
[SOCKET] Created listener socket fd=3
[SOCKET] SO_REUSEADDR set
[SOCKET] Bound to 0.0.0.0:6667
[SOCKET] Listening (backlog=15)
[INIT] Listener setup complete on port 6667
[INIT] Signal handlers installed (SIGINT, SIGTERM)
[INIT] Entering event loop...
[LOOP] Starting poll loop with 1 fd(s)
[LOOP] poll() waiting on 1 fd(s)...
```

### Terminal 2: Test with netcat
```bash
nc localhost 6667
```

Type commands:
```
PASS testpassword
NICK alice
USER alice 0 * :Alice
```

---

## 📊 Debug Output Explained

### Startup Phase
```
[INIT] Server constructed. Port: 6667, Password: testpassword
[SOCKET] Created listener socket fd=3
[SOCKET] SO_REUSEADDR set
[SOCKET] Bound to 0.0.0.0:6667
[SOCKET] Listening (backlog=15)
```
✅ **What this means**: Socket is ready, bound to port, listening for connections

### Client Connection
```
[ACCEPT] New client connected: fd=4 (total clients: 1)
```
✅ **What this means**: Client accepted, assigned fd=4, now 1 client connected

### Data Reception & Line Extraction
```
[RECV] fd=4 received 20 bytes
[DEBUG] Raw data (20 bytes): PASS<SP>testpassword<CR><LF>
[LINE] fd=4 extracted line #1
[CLIENT fd4] PASS testpassword
```
✅ **What this means**:
- `<SP>` = space character
- `<CR>` = carriage return (`\r`)
- `<LF>` = line feed (`\n`)
- Raw bytes show the exact protocol data
- Line extracted means we have a complete command (ending in `\r\n`)
- Command is passed to handler

### Partial Messages (Testing Buffer)
```
[RECV] fd=4 received 6 bytes
[DEBUG] Raw data (6 bytes): NICK<SP>a
[RECV] fd=4 received 7 bytes
[DEBUG] Raw data (7 bytes): lice<CR><LF>
[LINE] fd=4 extracted line #1
[CLIENT fd4] NICK alice
```
✅ **What this means**: Buffer correctly accumulates partial data until `\r\n` is received

### Client Disconnect
```
[POLL_ERROR] fd=4 got POLLHUP (client closed connection)
[DISCONNECT] Client fd=4 disconnected (total clients: 0)
```
✅ **What this means**: Connection properly cleaned up

---

## 🧪 Test Scenarios

### Test 1: Verify Line Ending Handling
```bash
# Terminal 1
./ircserv 6667 pass

# Terminal 2
printf "NICK test\r\n" | nc localhost 6667
```

**Watch for**: `[DEBUG]` output showing `<CR><LF>` at end

❌ **If you see** `[DEBUG] Raw data: NICK<SP>test` (no `<CR><LF>`), **problem**: Your client isn't sending proper line endings

---

### Test 2: Verify Multi-Client Handling
```bash
# Terminal 1
./ircserv 6667 pass

# Terminal 2
nc localhost 6667

# Terminal 3
nc localhost 6667

# Terminal 4
nc localhost 6667
```

**Watch for**: Each client gets unique fd (fd=4, fd=5, fd=6, etc.)

---

### Test 3: Verify Buffer Limits (8KB)
```bash
# Terminal 1
./ircserv 6667 pass

# Terminal 2
(python3 -c "print('X' * 9000)") | nc localhost 6667
```

**Watch for**: `[BUFFER_FULL] fd=N buffer overflow, disconnecting`

---

### Test 4: Verify Parsing Works
```bash
# Terminal 1
./ircserv 6667 pass

# Terminal 2 - Send multiple commands at once
printf "NICK alice\r\nUSER alice 0 * :Alice\r\nJOIN #test\r\n" | nc localhost 6667
```

**Watch for**: All 3 commands appear as separate `[LINE]` extractions

---

## 🔍 Debugging Workflow for 42 Evaluation

### Step 1: Run Server
```bash
./ircserv 6667 evaluatorpassword > server.log 2>&1 &
```

### Step 2: Send Commands from Another Terminal
```bash
# Test PASS
printf "PASS evaluatorpassword\r\n" | nc localhost 6667

# Test NICK
printf "NICK testuser\r\n" | nc localhost 6667

# Test USER
printf "USER testuser 0 * :Test User\r\n" | nc localhost 6667
```

### Step 3: Check Log
```bash
cat server.log
```

You'll see exactly:
- When client connected
- What raw bytes arrived
- How lines were extracted
- If parsing succeeded

---

## 🛠️ Customizing Debug Output

If you want to **disable** some debug lines temporarily:

**Comment out in `/home/alel-you/ft_irc/src/server_runtime.cpp`:**

```cpp
// Disable verbose poll loop logs
// std::cout << "[LOOP] poll() waiting on " << this->fds.size() << " fd(s)..." << std::endl;

// Disable raw byte dump
// debug_print_bytes("Raw data", buf, bytes);
```

---

## 🎯 Using with Real IRC Clients

### Install HexChat
```bash
sudo apt install hexchat
```

### Connect
1. Open HexChat
2. Network List → Add
3. Set:
   - Server: `127.0.0.1/6667`
   - Nick: `testuser`
   - Password: `evaluatorpassword`
4. Click Connect

### Watch Server Terminal
You'll see:
```
[ACCEPT] New client connected: fd=4
[RECV] fd=4 received 27 bytes
[DEBUG] Raw data (27 bytes): PASS<SP>evaluatorpassword<CR><LF>
[LINE] fd=4 extracted line #1
[CLIENT fd4] PASS evaluatorpassword
[RECV] fd=4 received 14 bytes
[DEBUG] Raw data (14 bytes): NICK<SP>testuser<CR><LF>
[LINE] fd=4 extracted line #2
[CLIENT fd4] NICK testuser
```

---

## ⚠️ Common Debugging Issues

| Issue | Debug Output to Look For | Likely Problem |
|-------|--------------------------|-----------------|
| Client won't connect | No `[ACCEPT]` message | Firewall or port conflict |
| Commands not recognized | `[RECV]` but no `[LINE]` | Missing `\r\n` or buffer issue |
| Multiple clients fail | Only 1 client shows `[ACCEPT]` | Poll loop not cycling correctly |
| Buffer error immediately | `[BUFFER_FULL]` right after accept | Client sending huge message |
| Client disappears | `[POLL_ERROR]` or `[DISCONNECT]` | Normal (client closed), expected |

---

## 📋 What Each Debug Prefix Means

| Prefix | Component | When | Use For |
|--------|-----------|------|---------|
| `[INIT]` | Startup | Before event loop | Checking socket setup |
| `[SOCKET]` | Socket ops | During `setup_listener()` | Verifying bind/listen |
| `[ACCEPT]` | Client accept | On new connection | Counting clients |
| `[RECV]` | Data arrival | When data received | Seeing raw bytes |
| `[DEBUG]` | Byte dump | With each RECV | Checking `\r\n` presence |
| `[LINE]` | Line extraction | When `\r\n` found | Verifying parsing |
| `[CLIENT fdN]` | Handler call | After line extracted | Seeing final command |
| `[LOOP]` | Poll cycle | Every iteration | Understanding timing |
| `[SIGNAL]` | Signal receive | On SIGINT/SIGTERM | Graceful shutdown |
| `[ERROR]` | Critical errors | On failures | Debugging crashes |

---

## 🎓 For 42 Evaluation

Your evaluator can now easily see:

1. ✅ **Server starts correctly** → See `[INIT]` messages
2. ✅ **Socket setup works** → See `[SOCKET]` messages
3. ✅ **Clients can connect** → See `[ACCEPT]` messages
4. ✅ **Data arrives** → See `[RECV]` + `[DEBUG]` messages
5. ✅ **Parsing works** → See `[LINE]` messages extracted
6. ✅ **Handles errors** → See `[ERROR]` or graceful disconnect
7. ✅ **Multi-client works** → See multiple fd numbers
8. ✅ **Buffer limits work** → See `[BUFFER_FULL]` when exceeded

Just let the evaluator connect with netcat or HexChat and all the debug output shows exactly what the server is doing.

---

## 📚 Next Phase

Once debugging is confirmed:
1. **Command Dispatcher**: Replace `[CLIENT fdN]` logger with real command routing
2. **Registration**: Add PASS/NICK/USER validation
3. **Channels**: Wire JOIN/PART/PRIVMSG to channel model
4. **Output Queue**: Add replies that don't block on slow clients

---

## 🔗 Links

- [README.md](README.md) – Full testing guide
- [REFACTOR_SUMMARY.md](REFACTOR_SUMMARY.md) – Architecture
- [docs/todo/next-steps.md](docs/todo/next-steps.md) – Development roadmap

