
# LeanUAE Source Strategy

## Purpose

LeanUAE needs a source strategy that preserves access to the best available UAE core code
without inheriting a foreign host environment by accident.

This document defines that strategy.

LeanUAE does **not** take either FS-UAE or WinUAE as a complete product template.  
It takes the **current shared UAE lineage** as a source of truth for emulator-side code,
while defining its own Leopard-native host architecture.

The current FS-UAE repository explicitly states that it is now a **superset of the WinUAE repository**
in order to make comparison, merging, and shared test builds easier. At the same time,
the official WinUAE project remains explicitly centered on a Windows build environment
with requirements such as Windows 7+, Visual Studio, and the Windows Driver Kit. :contentReference[oaicite:0]{index=0}

---

## Core Decision

LeanUAE uses the following rule:

> **Use the current shared FS-UAE / WinUAE code lineage as the source of truth for core and reusable emulator-side code.  
> Ignore both existing host environments.  
> Define the LeanUAE host from Leopard outward.**

This means:

- **FS-UAE is not adopted as a host model**
- **WinUAE is not adopted as a host model**
- **the shared modern UAE lineage is adopted as code source**
- **LeanUAE defines its own Leopard-native host**

---

## Why This Strategy

### 1. It keeps the core modern
The FS-UAE repository is now positioned as a shared code base that includes the WinUAE line,
which makes it the most practical place to follow current emulator-side evolution. :contentReference[oaicite:1]{index=1}

### 2. It avoids Windows-centered host assumptions
The official WinUAE project is explicitly documented around Windows requirements,
Visual Studio, and WDK-based build expectations. That makes it valuable as a technical reference,
but a poor default mental model for a Leopard-native host. :contentReference[oaicite:2]{index=2}

### 3. It avoids FS-UAE dependency-led host design
FS-UAE remains a cross-platform emulator project, and its build documentation reflects a broad dependency set.
LeanUAE does not treat that host stack as authoritative for Leopard. :contentReference[oaicite:3]{index=3}

### 4. It preserves reuse without surrendering architecture
This strategy allows LeanUAE to reuse emulator-side work where sensible,
without allowing either the FS-UAE host environment or the WinUAE host environment
to define module boundaries.

---

## Source Categories

LeanUAE should classify inherited code into three categories.

## Category A — Core Source of Truth

This category should stay as close as practical to the shared modern UAE lineage.

Typical examples:
- CPU-related logic
- chipset-related logic
- machine-state logic
- ROM semantics
- floppy semantics
- hardfile semantics
- save-state internals
- emulator timing logic that is not host-owned

### Rule
Code in this category is updated from the shared lineage whenever practical.

### Goal
Stay current without host contamination.

---

## Category B — Reusable Emulator-Side Support Code

This category contains code that is not pure core,
but may still be reusable if it is not strongly tied to SDL, Windows, launcher logic,
or other environment-specific assumptions.

Typical examples:
- helper logic around emulator data structures
- media parsing or handling code that is not host-framework-specific
- utility code that does not drag in foreign host abstractions

### Rule
Reuse is allowed, but only after responsibility is understood clearly.

### Goal
Reuse what is genuinely useful without importing structural baggage.

---

## Category C — Host Code

This category must be defined by LeanUAE itself.

Typical examples:
- application lifecycle
- windowing
- rendering setup
- audio output
- input collection
- timing services
- filesystem integration
- configuration
- UI glue
- framework integration

### Rule
Do not inherit this category from FS-UAE or WinUAE by default.

### Goal
Make Leopard the architectural authority.

---

## Repository-Level Interpretation

For LeanUAE, the FS-UAE repository should be treated primarily as:

- a **modern shared UAE code source**
- a **reuse quarry for emulator-side code**
- a **reference point for current lineage**

It should **not** be treated as:
- a host architecture mandate
- a dependency policy mandate
- a module-boundary mandate

The WinUAE repository should be treated primarily as:

- a **technical reference**
- a **parallel truth source for lineage and behavior**
- a **sanity check for shared-code intent**

It should **not** be treated as:
- a host structure template
- a build-system template
- a Leopard design authority

---

## Practical Extraction Rule

Whenever a file or subsystem is evaluated for reuse, ask these questions in order:

### 1. Is this core?
If yes, prefer keeping it close to the shared lineage.

### 2. Is this reusable support code?
If yes, inspect whether it depends on foreign host assumptions.

### 3. Is this host code?
If yes, default to rewriting it for LeanUAE.

### 4. If mixed, can it be split?
If yes, extract the reusable emulator-side part and redesign the host-facing remainder.

This rule is important because inherited files may mix:
- core responsibilities
- bridge responsibilities
- host responsibilities

LeanUAE should split such files whenever doing so improves architectural clarity.

---

## Host Independence Rule

LeanUAE must never inherit a host abstraction merely because it already exists upstream.

The question is not:

> “How do FS-UAE or WinUAE currently host this code?”

The question is:

> “What is the correct Leopard-native host boundary for this responsibility?”

This keeps LeanUAE from becoming a disguised port of someone else’s environment.

---

## Update Strategy

### Core-facing code
Track the shared lineage closely where feasible.

### Reuse-facing code
Import selectively and refactor as needed.

### Host-facing code
Own it fully inside LeanUAE.

This gives LeanUAE a balanced strategy:
- modern core
- selective reuse
- native host autonomy

---

## Decision Summary

LeanUAE does **not** choose between FS-UAE and WinUAE as complete products.

Instead, it adopts this split:

- **Core lineage:** current shared FS-UAE / WinUAE code base
- **Reuse source:** FS-UAE repository first, because it already carries the shared modern line
- **Host lineage:** LeanUAE only
- **Architecture authority:** Leopard

---

## Closing Statement

LeanUAE should inherit emulator knowledge, not foreign host identity.

Its source strategy is therefore:

- follow the shared UAE lineage for core truth
- reuse code where reuse is structurally honest
- reject inherited host assumptions
- let Leopard define the host