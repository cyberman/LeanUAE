
# LeanUAE Core Edge Plan

## Purpose

This document defines the first planned cleanup of the UAE core edge for LeanUAE.

LeanUAE does not aim to reinvent the emulator core.
It aims to **cut the host-facing edge more cleanly** so that a Leopard-native host can attach without inheriting unnecessary FS-UAE environment assumptions.

The current code already exposes a usable edge through `uae.h` and `amiga_main()`,
but that edge is still mixed with historical startup assumptions and host-oriented leftovers.

This plan defines:

- which existing edge areas are useful
- which startup responsibilities should stay
- which should move
- which should be replaced
- how LeanUAE should reshape the edge over time

---

## Current Edge Observation

The first important observation is this:

> **The UAE edge already exists, but it is not yet clean enough.**

Two main locations define the current edge:

- `od-fs/include/uae/uae.h`
- `od-fs/libamiga.cpp` via `amiga_main()`

`uae.h` already exposes a substantial host-facing API surface.
`amiga_main()` already acts as the practical entry point into the emulator core.

This is good news for LeanUAE:
the project does not need to invent a core edge from nothing.

However, the current edge is still mixed with:
- FS-UAE assumptions
- historical startup structure
- host-like preparation logic
- SDL-flavored error handling and display initialization leftovers

LeanUAE therefore treats the current edge as a **starting point**, not as a finished design.

---

## Edge Areas in `uae.h`

The current `uae.h` surface can be grouped into six meaningful edge areas and one deferred area.

## 1. Core Control Edge

### Existing responsibilities
- initialization
- main start
- reset
- pause
- quit

### Existing examples
- `amiga_init()`
- `amiga_main()`
- `amiga_reset(int hard)`
- `amiga_pause(int pause)`
- `amiga_quit()`

### LeanUAE view
This is a valid and important edge area.
LeanUAE should preserve this responsibility group, but eventually rename or wrap it more clearly as a core control contract.

### Planned target
- keep the responsibility
- reduce historical startup leakage
- make start semantics more explicit

---

## 2. Core Video Edge

### Existing responsibilities
- host video format setup
- render buffer attachment
- render/display callback registration
- RTG resolution capability input

### Existing examples
- `amiga_set_video_format(int format)`
- `amiga_add_rtg_resolution(int width, int height)`
- `amiga_set_render_buffer(...)`
- `amiga_set_render_function(...)`
- `amiga_set_display_function(...)`

### LeanUAE view
This is the most important host attachment zone for LeanUAE.
It already proves that the core is not entirely hardwired to one frontend model.

### Planned target
- preserve explicit frame attachment and callback-style control
- keep Leopard-specific rendering details out of this edge
- eventually express this as a cleaner video attachment contract

---

## 3. Core Audio Edge

### Existing responsibilities
- audio callback registration
- CD audio callback registration
- audio buffer sizing
- audio frequency setup
- flush behavior

### Existing examples
- `amiga_set_audio_callback(...)`
- `amiga_set_cd_audio_callback(...)`
- `amiga_set_audio_buffer_size(int size)`
- `amiga_set_audio_frequency(int frequency)`
- `amiga_set_audio_frequency_adjust(double adjust)`
- `amiga_flush_audio()`

### LeanUAE view
This is already close to what LeanUAE needs.
It allows the host to remain host-native while the core remains audio-output-aware without knowing Core Audio or any Leopard-specific API.

### Planned target
- preserve callback-driven audio edge structure
- reduce hidden policy where possible
- keep format expectations explicit

---

## 4. Core Media Edge

### Existing responsibilities
- floppy insertion/ejection
- CD-ROM insertion/ejection
- media path handling
- drive count queries
- media state callbacks

### Existing examples
- `amiga_floppy_set_file(...)`
- `amiga_floppy_get_file(...)`
- `amiga_cdrom_set_file(...)`
- `amiga_cdrom_eject(...)`
- `amiga_get_num_floppy_drives()`
- `amiga_get_num_cdrom_drives()`

### LeanUAE view
This is a useful host-control edge and should remain an explicit responsibility group.

### Planned target
- preserve media/device control at the edge
- keep host path policy outside the core
- let LeanUAE own file selection and path resolution

---

## 5. Core Option Edge

### Existing responsibilities
- option setting
- hardware option setting
- integer option setting
- application of pending options
- quickstart and path-related configuration

### Existing examples
- `amiga_set_option(...)`
- `amiga_set_option_and_free(...)`
- `amiga_set_hardware_option(...)`
- `amiga_set_int_option(...)`
- `amiga_set_initialized_and_apply_options()`
- `amiga_quickstart(...)`

### LeanUAE view
This edge is useful, but currently too mixed.
It combines valid emulator-side option ingress with historical startup/configuration baggage.

### Planned target
- keep a clear core option ingress
- move host-owned persistence and defaults out of the core edge
- narrow the option application path

---

## 6. Core Notification Edge

### Existing responsibilities
- log output
- LED updates
- media state notifications
- save/restore completion notifications
- GUI message callbacks
- host event callback registration

### Existing examples
- `amiga_set_log_function(...)`
- `amiga_set_led_function(...)`
- `amiga_set_media_function(...)`
- `amiga_on_save_state_finished(...)`
- `amiga_on_restore_state_finished(...)`
- `amiga_on_update_leds(...)`
- `amiga_set_gui_message_function(...)`

### LeanUAE view
This is a strong edge area because it supports loose coupling.
It should remain callback-oriented.

### Planned target
- preserve notification-style attachment
- group related notifications more cleanly over time
- avoid GUI-framework assumptions at the edge

---

## 7. Deferred / Non-V1 Edge Area

### Existing examples
- clipboard functions
- Lua-related hooks
- plugin or module lookup helpers
- serial/parallel extras
- miscellaneous host-ish convenience features

### LeanUAE view
These should not define the first clean edge.
They may matter later, but they are not part of the initial LeanUAE core-edge cleanup.

### Planned target
- defer
- do not let them drive early edge design
- revisit only after V1 host/core integration is stable

---

## `amiga_main()` Assessment

`amiga_main()` is the current practical start edge.
It is valuable, but mixed.

LeanUAE should therefore treat it as:
- **the correct place to begin**
- **not the final form of the edge**

The current function contains responsibilities that fall into three categories:

- keep
- move
- replace

---

## KEEP in `amiga_main()`

The following responsibilities are close enough to the core start path that they may remain temporarily near the entry point.

### `uae_register_emulation_thread();`
This is emulator-side thread registration.
It belongs close to emulation startup.

### `real_main(argc, argv);`
This is the actual descent into the existing emulator startup path.
It should remain the practical core start target until later refactoring proves otherwise.

### end-of-run flush/cleanup behavior
Examples:
- `filesys_flush_cache();`
- `fflush(NULL);`

These may remain temporarily until LeanUAE owns shutdown sequencing more explicitly.

---

## MOVE out of `amiga_main()`

The following responsibilities do not belong in the final thin core-start function.

### `keyboard_settrans();`
This is input-related preparation logic.
It may be necessary, but it should not live inside a generic core start entry forever.

### `max_uae_width = 8192;`
### `max_uae_height = 8192;`
These are display/render capability assumptions.
They belong in video capability setup or host/core video negotiation, not in the bare start edge.

### dummy argv construction
Using a hardcoded argv such as `"fs-uae"` is a historical compatibility measure.
It proves that the start path still expects a legacy “program main” world.

LeanUAE may temporarily neutralize this, but should ultimately reduce the dependence on fake argv-based startup.

---

## REPLACE in `amiga_main()`

The following items clearly do not belong in the long-term LeanUAE edge as currently expressed.

### `preinit_shm();`
This may remain necessary in some form, but it should become an explicit preparation stage with a clear responsibility,
not an opaque historical step buried inside the main start entry.

### `SDL_assert_release(preinit_shm_success);`
This must be replaced.
Even if the underlying condition remains important, SDL-flavored assertion logic has no place in the LeanUAE edge.

### `enumeratedisplays();`
This is the clearest example of host-oriented preparation living in the wrong place.
Display enumeration belongs in the host or in a clearly named capability-preparation stage, not in the general core start path.

---

## Planned Edge Restructuring

LeanUAE should gradually reshape the current edge into three layers.

## 1. `leuae_core_prepare(...)`

### Purpose
Handle core-adjacent preparation that should exist before actual start,
but should not be buried in the raw start function forever.

### Candidate responsibilities
- input translation preparation
- low-level preinit work if still needed
- explicit core capability preparation

### Rule
This layer should remain host-neutral at the interface.

---

## 2. `leuae_host_prepare(...)`

### Purpose
Handle Leopard-owned preparation before core start.

### Candidate responsibilities
- video capability setup
- display policy
- render path preparation
- audio path preparation
- input collection setup
- timing initialization

### Rule
This layer is Leopard-native.
It must not leak Leopard-native types into the core edge.

---

## 3. `leuae_core_start()`

### Purpose
Become the thin practical entry point into emulator execution.

### Candidate responsibilities
- emulation thread registration
- handoff into current core start path
- minimal orderly exit work

### Rule
This function should become smaller over time, not larger.

---

## Planned Edge Groups for LeanUAE

LeanUAE should treat the cleaned core edge as a set of responsibility groups.

## `core_control`
For:
- init
- start
- pause
- reset
- quit

## `core_video_edge`
For:
- render buffer attachment
- display/render callback setup
- video format selection
- RTG resolution capability input

## `core_audio_edge`
For:
- audio callback setup
- frequency and buffer size input
- audio flushing

## `core_media_edge`
For:
- floppy and CD media insertion/ejection
- device/media state queries
- media-related callbacks

## `core_option_edge`
For:
- core-facing option ingress
- hardware option ingress
- explicit option application

## `core_notify_edge`
For:
- log callbacks
- LED callbacks
- media callbacks
- save/restore completion callbacks
- host-visible notification callbacks

These groups do not need to become separate files immediately,
but they should become separate design responsibilities immediately.

---

## Immediate LeanUAE Rules

### Rule 1
Do not copy the entire current `uae.h` edge blindly.

### Rule 2
Do not widen the edge before LeanUAE V1 proves it necessary.

### Rule 3
Do not let startup leftovers continue to hide host preparation logic.

### Rule 4
Do not force Leopard details into the edge merely because Leopard is the primary host.

### Rule 5
Use LeanUAE-first cleanup as the basis for future host attachment discipline.

---

## Short-Term Engineering Goal

The first real LeanUAE core-edge task is not a broad rewrite.

It is this:

> **Make the current edge understandable, then make the startup edge thinner.**

That means:
- grouping `uae.h`
- identifying actual LeanUAE V1 edge needs
- shrinking `amiga_main()` toward a thinner core-start function
- moving host preparation toward explicit LeanUAE-owned stages

---

## Long-Term Value

This cleanup is valuable even beyond Leopard.

LeanUAE has primacy, but a cleaner UAE edge makes future native hosts easier to attach without forcing LeanUAE to design for them prematurely.

That is the intended balance:

- primary target first
- clean attachment points always

---

## Closing Statement

LeanUAE does not need a new emulator core.
It needs a cleaner host-facing edge around the existing one.

The current `uae.h` and `amiga_main()` code already provide the first anchor points.
The task now is to turn that mixed historical edge into a thinner, clearer, Leopard-first core boundary.