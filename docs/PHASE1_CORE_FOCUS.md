
# LeanUAE Phase 1 Core Focus

## Purpose

This document defines the **Phase 1 core focus set** for LeanUAE.

Its purpose is to prevent `src/core/` from becoming a new undifferentiated dump of inherited emulator code.

LeanUAE does not need all preserved core code to become active at once.

It needs a **small, deliberate working set** that supports the first real objective:

> **understand and reshape the current UAE core edge without getting lost in the full historical body of the code base**

This document divides the current `src/core/` files into:

- **HOT** — active Phase 1 focus
- **WARM** — likely relevant soon, but not first
- **COLD** — kept in core, but not active in Phase 1

---

## Phase 1 Objective

Phase 1 is not about broad emulator refactoring.

Phase 1 is about:

- understanding the current core edge
- tracing the startup path around `uae.h` and `amiga_main()`
- identifying host-facing control, video, audio, media, and input responsibilities
- keeping the initial working set small enough to remain understandable

The goal is not maximum coverage.

The goal is controlled understanding.

---

## HOT — Active Phase 1 Focus

These files form the current active working set.

They are the best first candidates for understanding and reshaping the current core edge.

## Startup / configuration / edge behavior
```text
src/core/cfgfile.cpp
src/core/autoconf.cpp
src/core/ini.cpp
src/core/logging.cpp
src/core/writelog.cpp
src/core/uaelib.cpp
src/core/uaeresource.cpp
src/core/traps.cpp
````

### Why

These files are close to:

- startup behavior
    
- option/config handling
    
- control-edge behavior
    
- host-visible interaction points
    

They are likely to matter directly when thinning the edge around `uae.h` and `amiga_main()`.

---

## Core execution / control path

```text
src/core/newcpu.cpp
src/core/newcpu_common.cpp
src/core/events.cpp
src/core/memory.cpp
src/core/custom.cpp
```

### Why

These files are part of the practical execution path and system behavior that the host edge eventually hands off to.

They matter because LeanUAE must understand:

- where core execution truly begins
    
- what startup assumptions are still mixed into the path
    
- where host preparation should stop and core execution should start
    

---

## Video / render edge

```text
src/core/gfxutil.cpp
src/core/gfxlib.cpp
src/core/drawing.cpp
src/core/picasso96.cpp
src/core/p96_blit.cpp
src/core/linetoscr_common.cpp
```

### Why

These files are central to the video-facing edge.

LeanUAE’s first host work will eventually need to understand:

- frame generation paths
    
- rendering assumptions
    
- video format and presentation boundaries
    
- where host video attachment should occur
    

These files are therefore part of the active Phase 1 study set.

---

## Audio edge

```text
src/core/audio.cpp
```

### Why

Audio is one of the core host attachment zones.  
This file is small enough in scope to justify immediate active focus.

---

## Media / disk / file edge

```text
src/core/disk.cpp
src/core/readdisk.cpp
src/core/cdrom.cpp
src/core/blkdev.cpp
src/core/blkdev_cdimage.cpp
src/core/hardfile.cpp
src/core/filesys.cpp
src/core/filesys_bootrom.cpp
src/core/zfile.cpp
src/core/zfile_archive.cpp
src/core/rommgr.cpp
```

### Why

LeanUAE V1 needs ROM, floppy, hardfile, and media understanding.

These files are part of the practical host/core edge because they affect:

- media loading
    
- path use
    
- file ownership
    
- boot content
    
- host-controlled media insertion
    

---

## Input edge

```text
src/core/inputdevice.cpp
src/core/keybuf.cpp
```

### Why

Input is one of the first mandatory host attachment zones.  
These files belong in the active set because LeanUAE must understand where host input enters the emulator side.

---

## WARM — Likely Relevant Soon

These files are not the first working set, but they are close enough to Phase 1 concerns that they may become active next.

## System / device-side extension zones

```text
src/core/devices.cpp
src/core/expansion.cpp
src/core/gayle.cpp
src/core/ide.cpp
src/core/idecontrollers.cpp
src/core/pci.cpp
src/core/rtc.cpp
src/core/savestate.cpp
src/core/scsi.cpp
src/core/scsidev.cpp
src/core/scsiemul.cpp
src/core/isofs.cpp
```

### Why

These are relevant to system shape and media/device behavior,  
but they are not the first edge-cutting tools.

They become important once the initial control/media edge is understood more clearly.

---

## Debug / introspection support

```text
src/core/debug.cpp
src/core/debuginfo.cpp
src/core/debugmem.cpp
```

### Why

Useful for deeper understanding and inspection, but not the first structural target.

---

## Input recording / replay vicinity

```text
src/core/inprec.cpp
src/core/inputrecord.cpp
```

### Why

Close to input behavior, but not part of the minimal initial edge understanding.

---

## Additional graphics-side relevance

```text
src/core/gfxboard.cpp
```

### Why

Potentially useful soon, but not needed before the primary video edge is understood.

---

## COLD — Preserved in Core, Not Active in Phase 1

These files remain in `src/core/`, but they should not consume attention during the first LeanUAE edge work.

## CPU bulk / variant mass

```text
src/core/cpudefs.cpp
src/core/cpuemu_*.cpp
src/core/cpummu.cpp
src/core/cpummu30.cpp
src/core/cpuopti.cpp
src/core/cpustbl.cpp
src/core/cputbl.h
```

### Why

This is important emulator mass, but it is not the first host-edge problem.

LeanUAE should avoid getting buried in CPU bulk before the startup and host-facing edge are understood.

---

## Blitter and related lower-level rendering detail

```text
src/core/blitfunc.cpp
src/core/blitfunc.h
src/core/blit.h
src/core/blitops.cpp
src/core/blittable.cpp
src/core/blitter.cpp
```

### Why

Important, but too detailed for the initial edge-focused phase.

---

## Line rendering variants beyond the shared path

```text
src/core/linetoscr_*.cpp
```

### Exception

`src/core/linetoscr_common.cpp` is HOT.

### Why

The variant-heavy rendering mass should remain parked until the basic video edge has been clarified.

---

## Floating-point and numerical support

```text
src/core/fpp.cpp
src/core/fpp_native.cpp
src/core/fpp_softfloat.cpp
src/core/calc.cpp
src/core/sinctable.cpp
```

### Why

Important core support, but not Phase 1 edge material.

---

## Filesystem / usage / database side areas

```text
src/core/fsdb.cpp
src/core/fsdb_unix.cpp
src/core/fsusage.cpp
src/core/diskutil.cpp
src/core/identify.cpp
```

### Why

These may matter later, but they are not where LeanUAE should begin cutting.

---

## Miscellaneous preserved-but-cold material

```text
src/core/aks.def
src/core/inputevents.def
src/core/patch.cpp
src/core/native2amiga.cpp
src/core/tabletlibrary.cpp
src/core/flashrom.cpp
src/core/vm.cpp
```

### Why

Preserved for correctness and future understanding, but not part of the current active edge work.

---

## Working Rules for Phase 1

### Rule 1

Do not widen the active set casually.

### Rule 2

A file should only move from WARM or COLD into HOT when a concrete edge question requires it.

### Rule 3

Do not treat “interesting” as equal to “currently relevant”.

### Rule 4

Do not let `src/core/` become a new undifferentiated mental dump.

### Rule 5

If a Phase 1 question can be answered without waking a COLD file, leave the COLD file asleep.

---

## Immediate Practical Use

When reading, tracing, or documenting Phase 1 work:

- start in HOT
    
- escalate to WARM only when needed
    
- leave COLD alone unless forced by evidence
    

This keeps LeanUAE aligned with its actual first goal:

> **make the core edge understandable before making it ambitious**

---

## Closing Statement

Phase 1 succeeds when the active working set remains small enough to understand,  
but rich enough to expose the real host-facing edge of UAE.

LeanUAE does not need all of core at once.

It needs the right first slice.