# Next Steps Roadmap

## Phase 1: Stabilize Core Runtime
- [ ] Wire startup flow from program entry to active server loop.
- [ ] Add clear socket lifecycle ownership in server class.
- [ ] Add robust startup failure handling and cleanup.
- [ ] Add shutdown path to close all descriptors cleanly.

Definition of done:
- Server starts, listens, accepts client connections, and exits cleanly without leaks.

## Phase 2: Build Connection Model
- [ ] Define client state model and storage container on server.
- [ ] Define channel state model and membership rules.
- [ ] Add input buffering and line parsing policy.
- [ ] Add output queue strategy for partial writes.

Definition of done:
- Multiple clients can connect concurrently and maintain independent state.

## Phase 3: Command Pipeline
- [ ] Add command tokenizer and normalization path.
- [ ] Add command dispatcher with registration map.
- [ ] Implement authentication gate policy before privileged commands.
- [ ] Add standardized numeric/error reply strategy.

Definition of done:
- Incoming lines route deterministically to handlers with consistent replies.

## Phase 4: Feature Delivery (IRC Commands)
- [ ] Implement PASS/NICK/USER registration sequence.
- [ ] Implement JOIN/PART/PRIVMSG behavior and channel fanout.
- [ ] Implement TOPIC/MODE/KICK/INVITE with permission checks.
- [ ] Add edge-case handling for invalid targets and missing params.

Definition of done:
- Basic IRC workflows function correctly between multiple test clients.

## Phase 5: Validation and Hardening
- [ ] Create repeatable manual test checklist with expected outcomes.
- [ ] Add integration-style smoke tests via netcat or IRC client scripts.
- [ ] Add stress checks for disconnects, malformed input, and rapid joins.
- [ ] Add basic observability logs for connect/auth/join/message/quit events.

Definition of done:
- Server remains stable under common failure and abuse scenarios.
