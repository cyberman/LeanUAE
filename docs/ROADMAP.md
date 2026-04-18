
# LeUAE Roadmap

## Project Direction

LeUAE is a **Leopard-native UAE host** built around the native contracts of **Mac OS X 10.5 Leopard**.

This roadmap does not measure success by feature parity with modern FS-UAE environments.  
It measures success by a different standard:

- correctness
- native host clarity
- stable execution
- clean module boundaries
- low dependency burden
- respect for Leopard as the platform contract

The roadmap is therefore ordered by **platform truth**, not by convenience features.

---

## Global Priorities

### Priority 1
Establish a correct and minimal Leopard-native host foundation.

### Priority 2
Run the UAE core reliably inside that host.

### Priority 3
Stabilize timing, audio, video, and input.

### Priority 4
Only then add comfort, tooling, and optional polish.

---

## Phase 0 — Ground Rules and Project Shape

### Goal
Freeze the architectural rules before implementation drifts into dependency-led thinking.

### Deliverables
- `HOST_CHARTER.md`
- `ARCHITECTURE.md`
- initial repository layout
- coding rules for host/core separation
- explicit V1 scope statement
- explicit non-goals statement

---

## Phase 1 — Repository and Skeleton

### Goal
Create a buildable project skeleton with clean module ownership.

### Tasks
- create source tree:
  - `src/core/`
  - `src/host/app/`
  - `src/host/video/`
  - `src/host/audio/`
  - `src/host/input/`
  - `src/host/time/`
  - `src/host/fs/`
  - `src/host/config/`
  - `src/bridge/`
  - `src/ui/`
- create public headers for each host unit
- create minimal bootstrap entry point
- add stub implementations for each host service
- verify that the project builds and links as an empty shell

---

## Phase 2 — Application Lifecycle and Main Loop Ownership

### Goal
Make `host.app` own the application lifecycle and the main loop.

### Tasks
- implement `host_app_init()`
- implement `host_app_run()`
- implement `host_app_request_quit()`
- implement `host_app_shutdown()`
- create a basic event pump
- define quit policy
- define startup/shutdown order
- establish one obvious top-level control loop

---

## Phase 3 — Filesystem and Configuration Foundation

### Goal
Provide stable host-side path handling and persistent configuration.

### Tasks
- implement `host_fs_init()`
- define config, data, and save paths
- implement `host_fs_read_file()`
- implement `host_fs_file_exists()`
- define `leuae_config`
- implement `host_config_set_defaults()`
- implement `host_config_load()`
- implement `host_config_save()`
- choose a simple, explicit config format

---

## Phase 4 — Video Foundation

### Goal
Bring up a native Leopard video path.

### Tasks
- create native window
- create OpenGL context
- implement `host_video_init()`
- implement `host_video_submit_frame()`
- implement `host_video_end_frame()`
- implement basic resize handling
- implement fullscreen toggle policy
- define frame pixel format contract
- ensure orderly shutdown of video resources

---

## Phase 5 — Input Foundation

### Goal
Bring keyboard and mouse into the host cleanly.

### Tasks
- implement `host_input_init()`
- implement `host_input_poll()`
- implement keyboard event translation
- implement mouse movement and button translation
- define input event structures
- add optional grab/capture mode handling
- define quit-event integration with main loop

---

## Phase 6 — Timing Foundation

### Goal
Establish reliable timing and pacing services.

### Tasks
- implement `host_time_now_us()`
- implement `host_time_sleep_us()`
- implement `host_time_sleep_ms()`
- define frame pacing policy
- define timing measurement policy
- centralize time access

---

## Phase 7 — Audio Foundation

### Goal
Bring up a Leopard-native audio output path.

### Tasks
- implement `host_audio_init()`
- define PCM contract
- implement `host_audio_start()`
- implement `host_audio_stop()`
- implement `host_audio_submit()`
- implement delay/buffer query
- define underrun behavior
- define shutdown behavior

---

## Phase 8 — Bridge Layer Introduction

### Goal
Connect host services to the emulator core through narrow interfaces.

### Tasks
- implement `bridge_init_from_config()`
- implement `bridge_shutdown()`
- implement `bridge_handle_input_event()`
- implement `bridge_submit_video_frame()`
- implement `bridge_submit_audio_samples()`
- define startup-time transfer of config into core-facing structures
- define ownership boundaries between host and core

---

## Phase 9 — Core Bring-Up

### Goal
Run the UAE core inside the Leopard host.

### Tasks
- select the first core startup path
- load ROM through host filesystem services
- load floppy image through host filesystem services
- pass frame output into `host.video`
- pass audio output into `host.audio`
- pass input into the core via bridge
- define orderly failure behavior for bad media or missing ROMs

---

## Phase 10 — Stabilization for V1

### Goal
Make the first usable version stable.

### Tasks
- improve startup diagnostics
- tighten shutdown behavior
- verify repeated start/quit cycles
- test windowed and fullscreen behavior
- verify configuration persistence
- verify keyboard and mouse behavior
- verify audio/video synchronization under load
- fix ownership and cleanup leaks
- reduce brittle code paths
- document known limitations

---

## V1 Definition

LeUAE V1 is complete when all of the following are true:

- ROM loading works
- floppy loading works
- hardfile loading works
- window creation works
- frame output works
- audio output works
- keyboard input works
- mouse input works
- timing is stable
- startup is orderly
- shutdown is orderly
- configuration persists

V1 does **not** require:

- launcher feature parity
- advanced overlays
- rich theming
- broad controller support
- convenience-heavy UI
- dependency-driven extras

---

## Phase 11 — Post-V1 Cleanup

### Goal
Reduce technical debt immediately after V1 before feature creep begins.

### Tasks
- remove temporary hacks
- simplify bridge translation paths
- improve naming consistency
- audit host/global state
- reduce duplication
- review config schema
- tighten error reporting
- review module interfaces for unnecessary leakage

---

## Phase 12 — Optional Native Comfort Features

### Goal
Add only those comfort features that still respect Leopard-native design.

### Candidate Tasks
- better file selection workflow
- screenshot support
- improved host-side settings UI
- optional controller support
- optional status display
- optional media insertion helpers
- optional display scaling improvements

### Rule
No comfort feature may redefine the architecture.  
Comfort must fit the host.  
The host must not bend around comfort.

---

## Phase 13 — Structural Refactoring of Inherited UAE Host Code

### Goal
Further split inherited code into Leopard-compatible responsibility units where necessary.

### Tasks
- identify mixed-responsibility inherited files
- split lifecycle logic from rendering logic
- split timing logic from audio/video logic
- split filesystem logic from config logic
- move host translation code into bridge units
- replace portability leftovers with explicit Leopard policy

---

## Closing Statement

LeUAE progresses in the right direction when each phase makes the host:

- more Leopard-native
- more explicit
- less dependency-led
- better separated by responsibility
- easier to reason about
- easier to stabilize

The roadmap is not a checklist for feature accumulation.  
It is a plan for building a correct Leopard application environment for the UAE core.