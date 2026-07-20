#pragma once

// Per-variant namespace selector for the OTRExporter globals.
//
// Main.cpp and VersionInfo.cpp define file-scope variables with EXTERNAL
// linkage (files, archive, archiveFileName, resourceVersions, ...). These
// sources are compiled into BOTH the OTRExporter_OoT and OTRExporter_MM static
// libraries, and both libraries are deliberately linked into the single
// `redship` executable (in-app ROM extraction needs both variants — see
// CMake/SingleExecutable.cmake in the superproject).
//
// Under Windows /FORCE:MULTIPLE the linker keeps ONE storage slot per global
// but BOTH per-variant dynamic initializers and BOTH atexit destructors. Every
// such global was therefore constructed twice at startup (leaking the first
// construction's allocations) and destroyed twice at exit — the second
// destruction walked already-freed heap and crashed EVERY normal exit with
// STATUS_HEAP_CORRUPTION (0xC0000374), including `redship --version`. That is
// Fault A (spencerduncan/redshipblueship#396).
//
// Wrapping the globals in a variant-specific namespace gives each library its
// own distinctly-mangled storage, so each variant constructs and destroys only
// its own copy: no shared slot, no double construction, no double destruction.
//
// The macro is keyed on the same GAME_MM / GAME_OOT definitions the CMake
// variant loop already sets for each target (OTRExporter/CMakeLists.txt). Every
// build path defines exactly one of them — including OTREXPORTER_SINGLE_GAME
// single-variant builds and the OTRExporter ALIAS target — so those
// configurations keep compiling unchanged.
#if defined(GAME_MM)
#define OTREXP_NS OTRExporterMM
#elif defined(GAME_OOT)
#define OTREXP_NS OTRExporterOoT
#else
#error "OTRExporter requires GAME_MM or GAME_OOT to select a per-variant namespace"
#endif
