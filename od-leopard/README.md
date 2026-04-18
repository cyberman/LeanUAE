# od-leopard — LeanUAE Leopard Platform Layer

## Philosophie

LeanUAE ersetzt den gesamten SDL/Deps-Stack von FS-UAE durch native
macOS Leopard (10.5.8 / PowerPC) APIs. Das Betriebssystem ist die
Bibliothek. Nichts anderes.

## Schichtmodell

WinUAE-Core (68k, Chipset, Blitter, CIA...) ↓ od-leopard/ (diese Schicht) ↓ Leopard native APIs ├── CoreAudio → Audio ├── Quartz 2D → Framebuffer / Rendering ├── AGL + OpenGL → OpenGL Context (optional) ├── Carbon Events → Input / Keyboard / Mouse └── POSIX / BSD → Filesystem, Threading, Time


## Dateien

| Datei                  | Ersetzt (FS-UAE)       | Leopard API       |
|------------------------|------------------------|-------------------|
| `leuae-video.c`        | `fsemu-sdlvideo.c`     | Quartz 2D / AGL   |
| `leuae-audio.c`        | `fsemu-sdlaudio.c`     | CoreAudio         |
| `leuae-input.c`        | `fsemu-sdlinput.c`     | Carbon Event Mgr  |
| `leuae-window.c`       | `fsemu-sdlwindow.c`    | Quartz / Carbon   |
| `target.h`             | `od-fs/target.h`       | —                 |
| `sysconfig-leopard.h`  | `od-fs/sysconfig-fs.h` | —                 |

## Ziele / Leitplanken

- AmigaOS 3.2+ als primäres Emulationsziel
- Leopard 10.5.8 / PowerPC G5 (iMac G5 iSight) als Host
- Keine externen Bibliotheken (kein SDL, kein FreeType, kein GLib)
- Fernziel: Portierbarkeit auf Haiku OS durch saubere HAL-Grenzen
