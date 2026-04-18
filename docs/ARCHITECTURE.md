
# LeUAE Architecture

## Overview

LeUAE is structured as a **native Leopard host layer** around the UAE emulator core.

The architecture is intentionally split into two major domains:

- **Core domain**
  - emulation logic
  - chipset/CPU/memory behavior
  - machine state
  - disk and ROM semantics
- **Host domain**
  - application lifecycle
  - video output
  - audio output
  - input handling
  - timing
  - file access
  - configuration

The core must not be shaped by foreign host-library assumptions.  
The host must not be shaped by Linux-oriented portability layers.  
The host follows Leopard.

---

## Architectural Goal

The goal is to provide a **clean, Leopard-native execution environment** for the UAE core.

This means:

- no SDL-centered architecture
- no dependency-led host design
- no fake portability where it harms clarity
- no monolithic host wrapper when Leopard already defines cleaner boundaries

The result should look like a native Leopard application hosting a UAE core, not like a transplanted cross-platform stack.

---

## Top-Level Structure

Recommended directory-level structure:

```text
src/
  core/
  host/
    app/
    video/
    audio/
    input/
    time/
    fs/
    config/
  bridge/
  ui/
````

### `core/`

Contains emulator-core code that should remain host-agnostic as far as practical.

### `host/`

Contains Leopard-specific host services.

### `bridge/`

Contains narrow translation code between core expectations and host services.

### `ui/`

Contains only the minimal user-facing logic needed by LeUAE itself.

This is not a launcher playground.  
It exists to support the host, not to dominate it.

---

## Host Modules

## `host.app`

### Responsibility

Owns application lifecycle and high-level control flow.

### Example interface

```c
int host_app_init(int argc, char **argv);
void host_app_run(void);
void host_app_request_quit(void);
void host_app_shutdown(void);
```

---

## `host.video`

### Responsibility

Owns all display-facing behavior.

### Example interface

```c
typedef struct host_video_desc {
    int width;
    int height;
    int fullscreen;
    int scale_mode;
} host_video_desc;

int host_video_init(const host_video_desc *desc);
void host_video_shutdown(void);
void host_video_begin_frame(void);
void host_video_submit_frame(const void *pixels, int width, int height, int pitch);
void host_video_end_frame(void);
void host_video_toggle_fullscreen(void);
void host_video_resize(int width, int height);
```

---

## `host.audio`

### Responsibility

Owns all sound output.

### Example interface

```c
typedef struct host_audio_desc {
    int sample_rate;
    int channels;
    int frames_per_buffer;
} host_audio_desc;

int host_audio_init(const host_audio_desc *desc);
void host_audio_shutdown(void);
int host_audio_start(void);
void host_audio_stop(void);
int host_audio_submit(const int16_t *samples, int frame_count);
int host_audio_get_delay_frames(void);
```

---

## `host.input`

### Responsibility

Owns all user input entering the emulator host.

### Example interface

```c
typedef enum host_input_event_type {
    HOST_INPUT_NONE = 0,
    HOST_INPUT_KEY_DOWN,
    HOST_INPUT_KEY_UP,
    HOST_INPUT_MOUSE_MOVE,
    HOST_INPUT_MOUSE_BUTTON_DOWN,
    HOST_INPUT_MOUSE_BUTTON_UP,
    HOST_INPUT_QUIT
} host_input_event_type;

typedef struct host_input_event {
    host_input_event_type type;
    int code;
    int x;
    int y;
    int dx;
    int dy;
    int button;
    unsigned int modifiers;
} host_input_event;

int host_input_init(void);
void host_input_shutdown(void);
int host_input_poll(host_input_event *event);
void host_input_set_grab(int enabled);
```

---

## `host.time`

### Responsibility

Owns timekeeping and pacing services.

### Example interface

```c
uint64_t host_time_now_us(void);
void host_time_sleep_us(uint64_t usec);
void host_time_sleep_ms(unsigned int msec);
```

---

## `host.fs`

### Responsibility

Owns host-side file and path services.

### Example interface

```c
typedef struct host_path_set {
    char config_dir[1024];
    char data_dir[1024];
    char save_dir[1024];
} host_path_set;

int host_fs_init(host_path_set *paths);
int host_fs_read_file(const char *path, void **data, size_t *size);
void host_fs_free_file_data(void *data);
int host_fs_file_exists(const char *path);
```

---

## `host.config`

### Responsibility

Owns LeUAE configuration as a host concern.

### Example interface

```c
typedef struct leuae_config {
    int fullscreen;
    int width;
    int height;
    int sample_rate;
    int frames_per_buffer;
    char rom_path[1024];
    char floppy_path[1024];
    char hardfile_path[1024];
} leuae_config;

int host_config_load(leuae_config *cfg);
int host_config_save(const leuae_config *cfg);
void host_config_set_defaults(leuae_config *cfg);
```

---

## `bridge/`

## Purpose

The bridge layer translates between:

- emulator-core expectations
    
- Leopard host services
    

### Example bridge interfaces

```c
int bridge_init_from_config(const leuae_config *cfg);
void bridge_shutdown(void);

void bridge_handle_input_event(const host_input_event *event);
void bridge_submit_video_frame(const void *pixels, int width, int height, int pitch);
int bridge_submit_audio_samples(const int16_t *samples, int frame_count);
```

---

## Control Flow

Recommended startup order:

1. `host_config_set_defaults()`
    
2. `host_config_load()`
    
3. `host_fs_init()`
    
4. `host_app_init()`
    
5. `host_video_init()`
    
6. `host_audio_init()`
    
7. `host_input_init()`
    
8. `bridge_init_from_config()`
    
9. enter main loop
    

Recommended shutdown order:

1. stop emulation activity
    
2. `bridge_shutdown()`
    
3. `host_input_shutdown()`
    
4. `host_audio_shutdown()`
    
5. `host_video_shutdown()`
    
6. `host_app_shutdown()`
    

---

## Main Loop Model

The main loop should be owned by `host.app`.

Recommended conceptual loop:

```c
while (!quit_requested) {
    while (host_input_poll(&ev)) {
        bridge_handle_input_event(&ev);
    }

    /* core step / scheduling */

    /* audio production -> bridge_submit_audio_samples() */
    /* video production -> bridge_submit_video_frame() */

    /* pacing via host.time */
}
```

---

## V1 Scope

LeUAE V1 needs:

- application startup
    
- configuration load/store
    
- ROM loading
    
- floppy and hardfile loading
    
- keyboard input
    
- mouse input
    
- video output
    
- audio output
    
- stable pacing
    
- clean shutdown
    

V1 does not need:

- feature-rich launcher
    
- skin systems
    
- advanced overlays
    
- broad controller matrix
    
- luxury UI frameworks
    
- convenience abstractions that obscure Leopard
    

---

## Final Statement

LeUAE should be built as a set of clear host responsibilities wrapped around the UAE core.

Its architecture must reflect Leopard’s native system structure, not the accidental structure of foreign dependency stacks.

The host is not a portability shell.  
The host is a Leopard application environment for UAE.

````

---

## ROADMAP.md

```md id="7rq0pm"
# ROADMAP.md

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
````

Ein kleiner zusätzlicher Feinschliff:  
`leuae_config` ist deutlich besser als `leoparduae_config`, und als Binary-Name würde ich ebenfalls schlicht **`leuae`** nehmen.