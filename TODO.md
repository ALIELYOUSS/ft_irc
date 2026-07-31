# RFC 1459 Compliance Fixes — TODO

- [x] Fix ERR_NOTREGISTERED (451) param in 7 command files (remove command name)
  - [x] src/commands/server_join.cpp
  - [x] src/commands/server_part.cpp
  - [x] src/commands/server_privmsg.cpp
  - [x] src/commands/server_topic.cpp
  - [x] src/commands/server_kick.cpp
  - [x] src/commands/server_invite.cpp
  - [x] src/commands/server_mode.cpp
- [x] Fix WRONG PASS -> 464 ERR_PASSWDMISMATCH in server_registration.cpp
- [x] Fix RPL_INVITING (341) param order in server_invite.cpp (channel before nick)
- [x] Fix RPL_CHANNELMODEIS (324) missing key/limit params in server_mode.cpp
- [x] Add RPL_NOTOPIC (331) on JOIN when no topic is set in server_join.cpp
- [x] Rebuild with `make re` to verify — SUCCESS (no compile errors)

