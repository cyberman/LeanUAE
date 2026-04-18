
# LeUAE Host Charter

## Purpose

LeUAE is a **Leopard-native UAE host** for **Mac OS X 10.5 Leopard** on **PowerPC and Intel**.

It does **not** aim to transplant the full modern FS-UAE dependency stack onto Leopard.  
Instead, LeUAE follows a different rule:

> **What Leopard already provides natively shall be used natively.**

LeUAE is therefore not defined by cross-platform libraries, but by the native service boundaries of Leopard itself.

---

## Core Principle

**The OS is the contract.**

LeUAE does not treat Linux-oriented or cross-platform library design as normative.  
It treats **Leopard’s own system architecture** as normative.

This leads to five design laws.

### 1. Native first
If Leopard provides a native service for a given responsibility, that native service takes priority over any third-party abstraction.

### 2. Host boundaries follow Leopard
Module boundaries must follow Leopard system responsibilities such as:
- application lifecycle
- windowing and events
- rendering
- audio
- input
- timing
- file I/O
- configuration

### 3. Core preserved, host reshaped
The emulator core remains the emulator core.  
The host side may be reorganized as needed to align with Leopard.

### 4. Portability is not a primary goal
Cross-platform portability is not a design target.  
If some portability falls out naturally later, that is acceptable, but it is not a requirement.

### 5. No dependency outranks a native framework
A foreign library must never become architecturally more important than a Leopard-native framework that already covers the same host responsibility.

---

## What LeUAE Is Not

LeUAE is **not** a strict “FS-UAE on Leopard” port in the conventional sense.

LeUAE is also **not** a feature-parity project for modern launcher, overlay, skin, and convenience layers.

Its priority is:

> **A correct Leopard-native host first. Convenience later.**

---

## Host Architecture

LeUAE should be split into host-side units according to Leopard-native responsibilities.

### `host.app`
Responsible for:
- process startup
- lifecycle management
- main loop ownership
- top-level error handling
- basic menu/application integration

Preferred basis:
- Cocoa where practical
- Carbon where Leopard/PPC pragmatics make it the better fit

### `host.video`
Responsible for:
- window creation
- display mode handling
- rendering path ownership
- frame presentation
- fullscreen/windowed transitions

Preferred basis:
- `OpenGL.framework`
- `Quartz.framework` / Core Graphics where appropriate for auxiliary display work

### `host.audio`
Responsible for:
- audio device setup
- stream/puffer management
- callback or pull model integration
- latency policy
- audio error handling

Preferred basis:
- `CoreAudio.framework`

### `host.input`
Responsible for:
- keyboard input
- mouse input
- pointer mode handling
- optional future controller support

Preferred basis:
- Carbon/Cocoa event system
- `IOKit.framework` where device-level handling is required

### `host.time`
Responsible for:
- timing
- frame pacing
- delays/sleeps
- monotonic measurement
- synchronization helpers

Preferred basis:
- Darwin/POSIX services
- system-native timing primitives

### `host.fs`
Responsible for:
- ROM path handling
- disk and hardfile loading
- save-state paths
- configuration file storage
- screenshots and related output files
- optional future file dialogs

Preferred basis:
- POSIX file I/O first
- native Leopard file APIs only where they add real value

### `host.config`
Responsible for:
- LeUAE-owned configuration model
- host-side defaults
- persistence format
- mapping between user-visible settings and host internals

This unit must not be forced into the assumptions of another host environment merely for source similarity.

---

## Dependency Mapping Rule

Every foreign dependency must be evaluated using this question first:

> **Which Leopard-native capability already covers the real responsibility of this dependency?**

Examples:

- SDL-like responsibilities should be split across `host.app`, `host.video`, `host.input`, and `host.time`
- OpenAL-like responsibilities should map to `host.audio` via Core Audio
- OpenGL helper layers should be replaced by direct Leopard OpenGL usage where feasible
- UI/font/overlay/image conveniences should be treated as optional host features, not automatic requirements

This rule exists to prevent dependency-driven architecture.

---

## UAE Restructuring Rule

If an inherited host-facing unit does not match Leopard system logic, it may be split or reorganized.

The question is not:

> “Which library was originally expected here?”

The question is:

> “What Leopard responsibility does this code actually belong to?”

This means UAE host-side code may be further decomposed into Leopard-compatible units whenever that improves architectural clarity.

---

## V1 Definition

LeUAE V1 is reached when the following work reliably on Leopard:

- ROM loading
- floppy and hardfile loading
- window creation
- frame output
- audio output
- keyboard input
- mouse input
- stable timing
- orderly startup
- orderly shutdown
- simple persistent configuration

V1 does **not** require:
- full modern launcher parity
- advanced overlay systems
- rich skinning
- broad controller support
- comfort features without a clear Leopard-native justification

---

## Engineering Standard

LeUAE work should always prefer:

- native Leopard frameworks over third-party abstractions
- explicit host responsibilities over convenience wrappers
- platform truth over source compatibility theater
- smaller, clearer host units over monolithic portability layers

The goal is not to imitate another environment’s assumptions.  
The goal is to let Leopard define the host.

---

## Closing Statement

**LeUAE is not a struggle to reassemble a foreign dependency forest on Leopard.  
It is a deliberate reconstruction of the UAE host around the native contracts of Mac OS X Leopard.**