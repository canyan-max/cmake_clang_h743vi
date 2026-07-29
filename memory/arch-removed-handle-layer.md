---
name: arch-removed-handle-layer
description: Removed Handle layer, merged into Device. Service currently thin passthrough.
metadata:
  type: project
---

Removed the entire 03_Platform/05_handle/ layer (draw/camera/led/storage handle × 8 files).

Device layer now owns static internal BSP drivers directly. No more void* params, no ops/handle structs, no handle-to-device delegation.

Current architecture: App → Service → Device → BSP → Impl

Service layer currently a thin passthrough over Device — no real business logic yet. User may remove it if desired; can add back when real application logic exists (OSD rendering, battery display, etc.).

**Why:** Handle layer provided ops-based indirection valuable for runtime polymorphism, which this fixed-hardware system doesn't need. Same compile-time isolation achieved without the boilerplate.

**How to apply:** When switching hardware (e.g. ST7789→SSD1306), only modify the relevant device_xxx.c file. Headers and Service layer remain unchanged.
