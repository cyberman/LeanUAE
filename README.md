# LeanUAE

**LeanUAE** is a surgically reduced **UAE architecture** with a **portable core edge** and **native host adapters**.

It does **not** aim to transplant the full FS-UAE environment onto old systems.
It does **not** treat SDL, FreeType, Python, GLib, or other foreign dependency stacks as architectural truth.
It does **not** begin from the frontend.

LeanUAE begins from a different premise:

> **Extract the real UAE core, define clean host-facing contracts, and let each host use its native capabilities directly.**

Leopard was the trigger.  
**LeanUAE is the project.**

---

## Project Status

**Early architecture and source-triage phase.**

The current work focuses on:

- identifying the real UAE core edge
- separating core, host, and bridge responsibilities
- reducing inherited environment assumptions
- defining the first host attachment contracts
- fixing a narrow, truthful first target before code growth

This is intentional.

LeanUAE prefers a correct narrow foundation over premature implementation sprawl.

---

## What LeanUAE Is

LeanUAE is:

- a **lean UAE refactoring effort**
- a **host-edge cleanup project**
- a **native-host-first emulator architecture**
- a **surgical extraction**, not a clean-room rewrite
- a way to let UAE attach more naturally to strong host operating systems

The core idea is simple:

- the **emulator core** should stay as host-agnostic as practical
- the **host** should attach through explicit, deterministic contracts
- each **operating system** should provide its own native backend
- foreign abstraction layers should not outrank host-native capabilities

In short:

> **LeanUAE does not port an environment. It extracts a core and lets hosts attach natively.**

---

## Why This Project Exists

Modern UAE-derived projects often carry a large amount of environment around the emulator core:

- frontend frameworks
- portability layers
- windowing abstractions
- audio abstractions
- font stacks
- launcher logic
- GUI layers
- convenience dependencies

That may be justified for broad cross-platform products.

It is not automatically the right fit when the actual goal is:

- a small truthful target
- a native host
- less dependency weight
- less host-side indirection
- cleaner ownership
- better system fit

LeanUAE exists because the real value lies in **freeing the UAE core from environment overgrowth** and defining a cleaner edge around it.

---

## Core Idea

LeanUAE treats the current UAE code base as something that must be **freed**, not rewritten.

This is not a clean-room project.
It is not a fantasy rewrite.
It is not an attempt to outgrow reality.

It is a **surgical project**.

The working assumption is:

- the real emulator core is still there
- it is surrounded by historical growth, mixed startup logic, portability leftovers, and dependency-driven host design
- those layers can be identified
- those edges can be cut more cleanly
- each successful cut can become a visible success step

That matters technically and personally.

LeanUAE is designed so progress can happen in **small, concrete, motivating increments**.

---

## Design Principles

### The OS is the contract
A host operating system defines the host architecture.
Not SDL.
Not generic portability convenience.
Not another platform’s assumptions.

### Native first
If the host OS already provides the needed capability, the host should use it natively.

### Primary target first
LeanUAE always starts from one real target, not five hypothetical ones.

### Clean attachment points always
The edge around the core should be cut so a host can attach cleanly without polluting emulator logic.

### Truth over breadth
LeanUAE prefers:
- verified scope over guessed compatibility
- fewer code paths over more untested ones
- explicit contracts over convenience-heavy abstraction
- smaller, clearer host code over general-purpose environment shells

### Abstraction follows real work
Interfaces should be created from actual host integration needs, not from imagined future portability.

---

## Architecture Direction

LeanUAE is converging on a structure like this:

```text
src/
  core/
  hostif/
  host/
    leopard/
    haiku/        # possible later
  bridge/
  ui/
````

### `core/`

Emulator-side logic and shared UAE lineage code.

### `hostif/`

The clean host-facing contract at the edge of the core.

### `host/`

Concrete native host implementations.

### `bridge/`

Translation seam between host contracts and emulator-side expectations.

### `ui/`

Minimal host-facing user logic only where actually needed.

The long-term goal is not “portable GUI sameness”.  
The long-term goal is:

> **one cleaner core edge, multiple native host implementations.**

---

## First Host Truth

LeanUAE is platform-neutral as a project idea, but it is **not** platform-neutral in implementation order.

The first host truth is intentionally narrow:

- **Host:** PowerPC on **Mac OS X 10.5.8**
    
- **Guest focus:** **AmigaOS 3.2+**
    

This first target is treated as **code truth**.

That means:

- no Intel support claim
    
- no earlier AmigaOS support claim
    
- no broad macOS compatibility claim
    
- no scope widening by implication
    
- no support promises without verification
    

The first host is narrow on purpose.

Leopard is not the identity of the project.  
It is the **first concrete proving ground**.

---

## Why Leopard First

Mac OS X 10.5.8 on PowerPC is a strong first host for this idea because it provides substantial native capabilities already:

- native OpenGL
    
- native audio stack
    
- native windowing and application frameworks
    
- native input and device services
    
- a strong Unix foundation
    

The point is not nostalgia.

The point is that Leopard is a host that already has much of what UAE needs, which makes it a good first place to prove that foreign dependency layers can be reduced or removed.

Leopard is therefore the **birthplace** of LeanUAE, not its conceptual limit.

---

## Future Host Possibilities

LeanUAE is **not** currently a multi-host implementation project.

However, the architecture is intentionally being cut so that future native hosts could exist without structural regret.

A likely future example is **Haiku**, because it also offers strong native kits and a system-defined application model.

That does **not** mean LeanUAE is currently targeting Haiku.  
It means LeanUAE is trying not to damage the future by cutting the core edge badly today.

The rule is:

> **Primary target first. Clean attachment points always.**

---

## Source Strategy

LeanUAE does not choose between FS-UAE and WinUAE as complete products.

Instead, it follows this split:

- **Core lineage:** current shared UAE / FS-UAE / WinUAE line
    
- **Reuse preference:** FS-UAE repository first
    
- **Host architecture authority:** the actual target OS
    
- **Project identity:** LeanUAE only
    

This means:

- emulator-side code should stay close to the modern UAE lineage where practical
    
- reusable support code may be adopted selectively
    
- host architecture must not be inherited blindly from FS-UAE or WinUAE
    

In short:

> **LeanUAE should inherit emulator knowledge, not foreign host identity.**

---

## Current Technical Focus

The first real technical focus is the existing UAE edge around:

- `uae.h`
    
- `amiga_main()`
    

Current findings suggest:

- the edge already exists
    
- but it is mixed
    
- startup logic still contains host-ish preparation and historical assumptions
    
- useful contracts are present, especially for control, video, audio, media, and notifications
    
- the first real work is to make that edge more understandable and thinner
    

The current approach is therefore:

1. understand the existing edge
    
2. group responsibilities
    
3. separate keep / move / replace zones
    
4. move host preparation outward
    
5. keep the actual core start thinner over time
    

That is the beginning of LeanUAE.

---

## Performance Philosophy

For LeanUAE, performance is not limited to raw emulation throughput.

Performance also includes:

- host-side efficiency
    
- responsiveness
    
- startup cost
    
- dependency weight
    
- memory footprint
    
- binary size
    
- clarity of execution paths
    
- overall fit to the host OS
    

A faster-feeling host with lower overhead is a legitimate performance gain, even if raw emulation speed remains unchanged.

LeanUAE therefore treats deep native integration as a performance strategy in its own right.

Reduced binary size is a desired consequence of that approach, but not the sole objective.

The real objective is a lighter, more responsive, better integrated UAE host.

---

## Non-Goals

LeanUAE is **not** currently trying to be:

- a full FS-UAE replacement
    
- a feature-parity clone of modern FS-UAE frontends
    
- a universal emulator framework
    
- a broad compatibility project
    
- a launcher-first project
    
- a clean-room rewrite
    
- a multi-host implementation from day one
    
- a support claim for platforms that cannot be verified
    

LeanUAE is intentionally strict.

---

## What Success Looks Like

A meaningful first LeanUAE version does **not** mean “everything modern FS-UAE can do”.

It means something much narrower and more honest:

- the core starts cleanly through a thinner edge
    
- the first native host attaches without dragging in a foreign environment
    
- ROM/media loading works
    
- frame output works
    
- audio output works
    
- keyboard and mouse work
    
- startup and shutdown are orderly
    
- the host remains understandable
    
- the architecture remains clean enough that another native host is still thinkable later
    

That is already a significant success.

---

## Documentation

The repository is expected to carry architecture documents such as:

- `HOST_CHARTER.md`
    
- `ARCHITECTURE.md`
    
- `ROADMAP.md`
    
- `SOURCE_STRATEGY.md`
    
- `HOST_INTERFACE.md`
    
- `CORE_EDGE_PLAN.md`
    
- `TARGET_SCOPE.md`
    

These documents are part of the engineering discipline of the project.

LeanUAE is being shaped deliberately before it is allowed to spread.

---

## License

**GPL-2.0**

LeanUAE follows the licensing requirements of the inherited FS-UAE / UAE code base.

---

## Guiding Sentences

> **Leopard was the trigger. LeanUAE is the project.**

> **LeanUAE does not port an environment. It extracts a core and lets hosts attach natively.**

> **Primary target first. Clean attachment points always.**

---

## Current State

Work in progress.

Architecture first.  
Scope first.  
Truth first.

---
