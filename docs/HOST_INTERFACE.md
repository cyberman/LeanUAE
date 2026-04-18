
# LeanUAE Host Interface

## Purpose

This document defines the first host-facing attachment points at the edge of the UAE core.

Its purpose is not to create a universal portability framework.
Its purpose is to define a small, explicit set of contracts that allow LeanUAE to host the core cleanly on Leopard.

LeanUAE has primacy.

However, these interfaces should be cut cleanly enough that a future native host could attach without structural regret.

> **Primary target first. Clean attachment points always.**

---

## Design Intent

The host interface exists to separate three concerns:

- **core logic**
- **host implementation**
- **translation between them**

The core should not know Leopard details.
The Leopard host should not leak its framework assumptions into the core.
The interface should stay small, explicit, and responsibility-driven.

---

## Scope of V1

LeanUAE V1 needs host attachment points for:

- lifecycle control
- video frame delivery
- audio sample delivery
- input event delivery
- timing services
- media/file access
- configuration ingress

Nothing more should be abstracted until real host work proves it necessary.

---

## Interface Rules

### 1. No speculative abstraction
Do not introduce an interface just because another platform might need it later.

### 2. Explicit data contracts
Frame, audio, input, and config data must be explicit in shape and ownership.

### 3. Host-neutral at the boundary
The boundary must not expose Leopard-specific types.

### 4. Leopard-first implementation
The first implementation targets Leopard and may shape the interface,
but it must not hard-code Leopard framework types into the contract.

### 5. Small surface area
Prefer fewer, clearer contracts over broad generic wrappers.

---

## Host Interface Areas

## 1. Lifecycle Control

### Purpose
Control startup, run state, and shutdown at a host-neutral boundary.

### Contract
```c
int hostif_init(int argc, char **argv);
void hostif_run(void);
void hostif_request_quit(void);
void hostif_shutdown(void);
````

### Notes

This interface represents host lifecycle ownership.  
It does not imply any specific event system.

---

## 2. Video Frame Delivery

### Purpose

Allow the core or bridge to hand finished frame data to the host.

### Contract

```c
typedef struct hostif_video_frame {
    const void *pixels;
    int width;
    int height;
    int pitch;
    int format;
} hostif_video_frame;

int hostif_video_init(int width, int height, int fullscreen);
void hostif_video_submit(const hostif_video_frame *frame);
void hostif_video_present(void);
void hostif_video_shutdown(void);
```

### Notes

The contract is about frame ownership and presentation.  
It must not expose OpenGL, Quartz, or window handles.

---

## 3. Audio Sample Delivery

### Purpose

Allow the core or bridge to hand PCM audio to the host.

### Contract

```c
typedef struct hostif_audio_desc {
    int sample_rate;
    int channels;
    int frames_per_buffer;
} hostif_audio_desc;

int hostif_audio_init(const hostif_audio_desc *desc);
int hostif_audio_submit(const int16_t *samples, int frame_count);
void hostif_audio_start(void);
void hostif_audio_stop(void);
void hostif_audio_shutdown(void);
```

### Notes

The contract should remain PCM-first and explicit.  
No hidden format negotiation in V1.

---

## 4. Input Event Delivery

### Purpose

Allow the host to provide translated input events to the bridge/core side.

### Contract

```c
typedef enum hostif_input_event_type {
    HOSTIF_INPUT_NONE = 0,
    HOSTIF_INPUT_KEY_DOWN,
    HOSTIF_INPUT_KEY_UP,
    HOSTIF_INPUT_MOUSE_MOVE,
    HOSTIF_INPUT_MOUSE_BUTTON_DOWN,
    HOSTIF_INPUT_MOUSE_BUTTON_UP,
    HOSTIF_INPUT_QUIT
} hostif_input_event_type;

typedef struct hostif_input_event {
    hostif_input_event_type type;
    int code;
    int x;
    int y;
    int dx;
    int dy;
    int button;
    unsigned int modifiers;
} hostif_input_event;

int hostif_input_init(void);
int hostif_input_poll(hostif_input_event *event);
void hostif_input_set_grab(int enabled);
void hostif_input_shutdown(void);
```

### Notes

The host owns native event collection.  
The contract exports only translated input events.

---

## 5. Timing Services

### Purpose

Provide time queries and sleep primitives without leaking platform specifics.

### Contract

```c
uint64_t hostif_time_now_us(void);
void hostif_time_sleep_us(uint64_t usec);
void hostif_time_sleep_ms(unsigned int msec);
```

### Notes

All timing should flow through this interface to prevent timing fragmentation.

---

## 6. Media and File Access

### Purpose

Provide host-side access to ROMs, floppy images, hardfiles, save data, and config paths.

### Contract

```c
typedef struct hostif_paths {
    char config_dir[1024];
    char data_dir[1024];
    char save_dir[1024];
} hostif_paths;

int hostif_fs_init(hostif_paths *paths);
int hostif_fs_read_file(const char *path, void **data, size_t *size);
void hostif_fs_free_file_data(void *data);
int hostif_fs_file_exists(const char *path);
```

### Notes

This contract is intentionally small.  
It should not grow into a general virtual filesystem unless real need appears.

---

## 7. Configuration Ingress

### Purpose

Provide host-owned configuration to startup and bridge code.

### Contract

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

void hostif_config_set_defaults(leuae_config *cfg);
int hostif_config_load(leuae_config *cfg);
int hostif_config_save(const leuae_config *cfg);
```

### Notes

The host owns persistent user configuration.  
The core consumes only what it needs.

---

## Bridge Position

The bridge sits between host interfaces and emulator-side logic.

It is responsible for:

- translating input events into emulator actions
    
- passing frame buffers to video submission
    
- passing PCM samples to audio submission
    
- mapping configuration into emulator startup state
    

The bridge is not the host.  
The bridge is not the core.  
It is the translation seam.

---

## Non-Goals

This document does not define:

- a universal multi-platform framework
    
- launcher interfaces
    
- skinning interfaces
    
- controller abstraction beyond what LeanUAE V1 actually needs
    
- rich UI service layers
    
- speculative future host features
    

---

## Final Rule

The host interface must stay honest.

It exists to make LeanUAE possible without binding the UAE edge unnecessarily to Leopard internals.  
It does not exist to predict every future port.

LeanUAE is the primary implementation.  
Future reuse is a consequence of a clean cut, not the primary design goal.

## Minimal C stubs

### `src/main.c`

```c
#include "host/app/host_app.h"

int main(int argc, char **argv)
{
    if (host_app_init(argc, argv) != 0) {
        return 1;
    }

    host_app_run();
    host_app_shutdown();

    return 0;
}
````

### `src/host/app/host_app.h`

```c
#ifndef LEUAE_HOST_APP_H
#define LEUAE_HOST_APP_H

int host_app_init(int argc, char **argv);
void host_app_run(void);
void host_app_request_quit(void);
void host_app_shutdown(void);

#endif
```

### `src/host/app/host_app.c`

```c
#include "host_app.h"
#include "../time/host_time.h"

static int g_quit_requested = 0;

int host_app_init(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    g_quit_requested = 0;
    return 0;
}

void host_app_run(void)
{
    while (!g_quit_requested) {
        host_time_sleep_ms(10);
        g_quit_requested = 1;
    }
}

void host_app_request_quit(void)
{
    g_quit_requested = 1;
}

void host_app_shutdown(void)
{
}
```

### `src/host/time/host_time.h`

```c
#ifndef LEUAE_HOST_TIME_H
#define LEUAE_HOST_TIME_H

#include <stdint.h>

uint64_t host_time_now_us(void);
void host_time_sleep_us(uint64_t usec);
void host_time_sleep_ms(unsigned int msec);

#endif
```

### `src/host/time/host_time.c`

```c
#include "host_time.h"

#include <sys/time.h>
#include <time.h>

uint64_t host_time_now_us(void)
{
    struct timeval tv;

    gettimeofday(&tv, 0);

    return ((uint64_t)tv.tv_sec * 1000000ULL) + (uint64_t)tv.tv_usec;
}

void host_time_sleep_us(uint64_t usec)
{
    struct timespec ts;

    ts.tv_sec = (time_t)(usec / 1000000ULL);
    ts.tv_nsec = (long)((usec % 1000000ULL) * 1000ULL);

    nanosleep(&ts, 0);
}

void host_time_sleep_ms(unsigned int msec)
{
    host_time_sleep_us((uint64_t)msec * 1000ULL);
}
```

---
