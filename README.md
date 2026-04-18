# LeUAE

**LeUAE** is a **Leopard-native UAE host** for **PowerPC Macs running Mac OS X 10.5.8**.

It does **not** aim to become a broad compatibility project.
It does **not** aim to transplant the full modern FS-UAE environment onto Leopard.
It does **not** treat foreign dependency stacks as architectural truth.

LeUAE exists to host the UAE core through a **tight, native, Leopard-first design** with a **cleaner host-facing edge** and a **strictly limited target scope**.

---

## Project Status

**Early architecture and source-triage phase.**

The project direction, scope, and host strategy are being fixed before implementation expands.
This is intentional.

LeUAE prefers a correct narrow foundation over premature code growth.

---

## Project Goals

LeUAE is built around these goals:

- create a **native Leopard host** for the UAE core
- reduce dependency weight and host-side indirection
- keep the project tightly scoped and verifiable
- treat **Mac OS X 10.5.8 on PowerPC** as fixed host truth
- treat **AmigaOS 3.2+** as fixed guest truth
- cut cleaner attachment points at the edge of the UAE core
- avoid inheriting foreign host identity from FS-UAE or WinUAE

LeUAE is not only about emulation speed.
It is also about:

- host efficiency
- responsiveness
- startup cost
- memory footprint
- dependency burden
- binary size
- overall fit to Leopard

---

## Non-Goals

LeUAE is **not** intended to be:

- a full modern FS-UAE replacement
- a feature-parity clone of the FS-UAE frontend stack
- a broad portability framework
- an Intel Leopard project
- a compatibility project for AmigaOS versions earlier than 3.2
- a launcher-first or convenience-first emulator environment

LeUAE is intentionally narrow.

---

## Target Scope

LeUAE currently targets:

- **Host:** PowerPC on **Mac OS X 10.5.8 Leopard**
- **Guest focus:** **AmigaOS 3.2+**

Anything outside this scope is out of project truth unless explicitly revisited and verified later.

This means:

- no Intel support claim
- no earlier AmigaOS support claim
- no broad macOS support claim
- no compatibility promises without verification

For LeUAE, **truthful support** matters more than broad support claims.

---

## Core Strategy

LeUAE does not choose between FS-UAE and WinUAE as complete products.

Instead, it uses this split:

- **Core lineage:** current shared FS-UAE / WinUAE code line
- **Reuse preference:** FS-UAE repository first
- **Host architecture authority:** Leopard
- **Primary implementation:** LeUAE only

This means:

- emulator-side code should stay close to the modern shared UAE line where practical
- reusable support code may be adopted selectively
- the host environment must be defined by Leopard, not inherited from FS-UAE or WinUAE

In short:

> **LeUAE should inherit emulator knowledge, not foreign host identity.**

---

## Design Principles

LeUAE follows these principles:

### The OS is the contract
Leopard defines the host architecture.
Not SDL.
Not Windows.
Not generic cross-platform convenience layers.

### Native first
If Leopard already provides the needed host capability, that native capability has priority over third-party abstraction.

### Primary target first
LeUAE has primacy.
Future reuse is welcome, but only as a consequence of a clean cut.

### Clean attachment points always
The UAE edge should be shaped so that the host can attach cleanly without polluting the core.

### Truth over breadth
LeUAE prefers:
- verified scope over guessed scope
- fewer code paths over more untested ones
- explicit structure over convenience-heavy abstraction

---

## Why This Project Exists

Modern UAE-derived projects often carry a large amount of frontend, portability, and dependency structure around the emulator core.

That is useful for broad cross-platform products.
It is not automatically the right fit for a tightly scoped Leopard/PPC project.

LeUAE exists because a different question is being asked:

> **What would a truthful, Leopard-native UAE host look like if Leopard itself defined the boundaries?**

That question leads to a different architecture:

- smaller host surface
- fewer foreign assumptions
- cleaner host/core separation
- tighter system fit
- less dependency-led complexity

---

## Planned Architecture

LeUAE is being structured around these domains:

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

Emulator-side logic and shared UAE lineage code.

### `host/`

Leopard-native host services only.

### `bridge/`

Translation seam between host-facing contracts and emulator-side expectations.

### `ui/`

Minimal LeUAE user-facing logic only.

The project is intentionally split so the host can be native without forcing Leopard details into the core.

---

## Current Edge Focus

The first major technical focus is the existing UAE host-facing edge around:

- `uae.h`
    
- `amiga_main()`
    

The project has already identified that:

- the current edge **exists**
    
- but it is **not yet clean enough**
    
- and it still contains mixed startup and host-preparation assumptions
    

LeUAE therefore focuses on:

- understanding the current edge
    
- grouping existing edge responsibilities
    
- thinning the startup path
    
- moving host preparation out of mixed historical entry code
    
- preserving useful emulator-side contracts without inheriting the full FS-UAE environment
    

---

## Current Source Triage Direction

Initial triage of the FS-UAE tree shows a useful split:

### HOST / environment-heavy

- `od-fs/fsemu/`
    
- large parts of `od-fs/fsuae/`
    

### REUSE / host-edge relevant

- `od-fs/include/uae/uae.h`
    
- `od-fs/libamiga.cpp`
    
- `od-fs/libuae/` for reference and structure study
    

### CORE / low-level

- emulator-side code under the shared UAE lineage
    
- `od-fs/machdep/ppc/`
    
- `ppc/` as PPC-related emulator-side logic, not host code
    

This confirms the current LeUAE direction:

> **Do not port the FS-UAE environment.  
> Cleanly re-host the UAE edge.**

---

## Documentation

The repository is expected to carry the following design documents:

- `HOST_CHARTER.md`
    
- `ARCHITECTURE.md`
    
- `ROADMAP.md`
    
- `SOURCE_STRATEGY.md`
    
- `HOST_INTERFACE.md`
    
- `CORE_EDGE_PLAN.md`
    
- `TARGET_SCOPE.md`
    

These documents are not decoration.  
They are part of the engineering discipline of the project.

LeUAE is being shaped deliberately before it is allowed to grow.

---

## Development Philosophy

LeUAE favors:

- small explicit responsibilities
    
- clear ownership
    
- narrow interfaces
    
- host truth over source compatibility theater
    
- verifiable targets over assumed support
    
- cleanup of historical edge mixing
    
- real host work over speculative abstraction
    

This is a Leopard-first project.  
It is allowed to be strict.

---

## What Success Looks Like

LeUAE V1 does **not** mean “everything FS-UAE can do”.

LeUAE V1 means:

- ROM loading works
    
- floppy loading works
    
- hardfile loading works
    
- window creation works
    
- frame output works
    
- audio output works
    
- keyboard input works
    
- mouse input works
    
- startup is orderly
    
- shutdown is orderly
    
- configuration persists
    
- the host remains native and understandable
    

That is enough for a meaningful first version.

---

## Scope Warning

If you are looking for:

- broad platform support
    
- modern cross-platform convenience stacks
    
- frontend feature parity
    
- unverified Intel support
    
- generalized portability promises
    

this project is probably not the right fit.

If you are interested in:

- native Leopard design
    
- PowerPC truth
    
- tight host/core boundaries
    
- disciplined source reuse
    
- a smaller and cleaner UAE host
    

then LeUAE is exactly that kind of project.

---

## Guiding Sentence

> **LeUAE is not a struggle to rebuild a foreign dependency forest on Leopard.  
> It is a deliberate reconstruction of the UAE host around the native contracts of Mac OS X 10.5.8 on PowerPC.**

---

## License

## License

LeUAE is released under the **GNU General Public License v2.0 (GPL-2.0)**,
following the licensing conditions of the inherited FS-UAE / UAE code base.

Source provenance and inherited code origins should be documented clearly as the project evolves.

---

## Current State

Work in progress.

Architecture first.  
Scope first.  
Truth first.
