# TinySAAI Developer Guide

The operational side of contributing: getting the code, the toolchain, building,
flashing, recovering a unit, and the conventions for console commands. Policy is
in [`CONTRIBUTING.md`](../CONTRIBUTING.md) and [`GOVERNANCE.md`](../GOVERNANCE.md);
principles in [`CONSTITUTION.md`](../CONSTITUTION.md). What TinySAAI adds is in
[`ai/README.md`](../ai/README.md).

---

## Getting the code

```sh
git clone -b ai https://github.com/aethersdr/TinySAAI.git
cd TinySAAI
git submodule update --init --recursive
```

`ai` is the fork's branch. `main` is an untouched copy of upstream
([erikkaashoek/tinySA](https://github.com/erikkaashoek/tinySA)); add upstream as
a remote to compare against it:

```sh
git remote add upstream https://github.com/erikkaashoek/tinySA.git
git fetch upstream
```

## Toolchain

| Item | Version | Notes |
|---|---|---|
| `arm-none-eabi-gcc` with newlib | Recorded with each image (16.2 for the images so far) | Upstream releases use GCC 11.3. Changing the project's reference compiler is an RFC. |
| `dfu-util` | Any recent release | For flashing |
| Python 3 | Standard library only | `ai/flash.py`, `ai/make_coverage.py` |

Packages: on Arch, `arm-none-eabi-gcc arm-none-eabi-newlib dfu-util`; on
Debian/Ubuntu, `gcc-arm-none-eabi libnewlib-arm-none-eabi dfu-util`.

## Building

```sh
make TARGET="F303"        # tinySA Ultra / Ultra+; the image is build/tinySA4.bin
```

The version string comes from `git describe` and ends in `-TinySAAI`; a build
from a tree with uncommitted changes says `-dirty`. Images flashed onto a bench
unit or archived are built from a clean, committed tree.

The original tinySA (`make` with no `TARGET`) does not currently link (RAM
overflow); see the [roadmap](../ROADMAP.md).

### Showing that a change leaves the code unchanged

For refactors, layout changes and upstream merges that should not change the
firmware, build before and after with the same version string and compare:

```sh
make TARGET="F303" GIT_VERSION=compare && cp build/tinySA4.bin /tmp/before.bin
# … apply the change, then: rm -rf build .dep
make TARGET="F303" GIT_VERSION=compare && cmp -l build/tinySA4.bin /tmp/before.bin
```

Only the embedded build time (a few bytes) should differ. `arm-none-eabi-strings`
on both images shows which.

## Flashing

```sh
python3 ai/flash.py build/tinySA4.bin
```

It sends `reset dfu` over the console, flashes with `dfu-util`, waits for the
unit to restart and prints the version. It refuses an image larger than the
flash region in `boards/STM32F303xC.ld` (240 KB), so the configuration and
calibration in the last 16 KB of flash are never touched. You need permission to
open the serial port and the DFU device (USB `0483:df11`), for example through a
udev rule.

**Before flashing a unit:** claim it (Principle X); make sure its stock image and
the last good image are archived with SHA-256 checksums; record the stored
calibration (`leveloffset`, `correction <table>` for each table, `freq_corr`) so
it can be compared afterwards.

**After flashing:** check `version`, compare the stored calibration with the
record, and run the CAL self-check (CAL to RF cable fitted, 30 MHz tone at about
−35.6 dBm).

### Recovery

The STM32 ROM bootloader is always reachable by hand: power off, hold the jog
button, power on. Then `python3 ai/flash.py --already-dfu <image>`, or flash the
official image to go back to stock firmware. Flashing keeps the stored
calibration either way.

## Layout

See the README's Layout section. In short: firmware core in `src/`, drivers in
`src/drivers/`, fonts in `src/fonts/`, board definitions and linker scripts in
`boards/`, ChibiOS and FatFs in `lib/`, TinySAAI's own code and tools in `ai/`.
`chconf.h`, `halconf.h` and `mcuconf.h` stay in the root because ChibiOS's build
reads them from there.

## Console commands

### Where things are

| What | Where |
|---|---|
| Command table | `src/main.c`, the `commands[]` array; TinySAAI entries are marked `// TinySAAI` |
| Upstream commands, and `calibrate`, `state` | `src/sa_cmd.c` (included into `src/main.c`) |
| `ui`, `param`, `measure`, `readout`, `band`, `limit` | `ai/ai_ui.c` (included at the end of `src/ui.c`) |
| Guards: items refused, items needing `confirm` | `ai/ai_ui.c`, the single source of truth (Principle V) |
| Line limits | `VNA_SHELL_MAX_ARGUMENTS` and `VNA_SHELL_MAX_LENGTH` in `src/main.c` |

Command flags: `CMD_WAIT_MUTEX` runs the command in the sweep thread,
`CMD_RUN_IN_LOAD` allows it in load scripts, `CMD_RUN_IN_UI` lets it run while a
screen waits, `CMD_DISK_OK` allows it in USB-disk mode.

### Conventions

- **Replies are for software.** One fact per line, `key value` or
  `field|field|…`, no decoration. Document the format in `ai/README.md`.
- **Invalid input:** reply `error: <what was wrong and what is allowed>` and
  change nothing (Principle VII). Wrong argument count: `usage: …`.
- **Valid input behaves as upstream** (Principle II). New behaviour is a new
  command or argument.
- **Guards:** anything that would wait for a person at the screen, cut the USB
  console or erase the configuration is refused. Anything that changes stored
  calibration or configuration needs a trailing `confirm`. Add new items to the
  existing tables; never handle them in a second place.
- **Stored structures are never changed** (Principle II). Check the image size
  against the flash region before you flash.
- **Mark** TinySAAI code in upstream files with a `TinySAAI:` comment, and keep
  such edits minimal; put new code in `ai/`.

### Regenerating `ai/COVERAGE.md`

`ai/make_coverage.py` builds the coverage list from the instrument's own replies.
Capture them from a unit running the new image:

1. In analyzer mode, send `ui tree`, then `param`, and save both replies in one
   file, separated by a line `### param`.
2. In low-output (generator) mode with the output off, send `ui tree` and save the
   reply to a second file.
3. Run:

```sh
python3 ai/make_coverage.py ANALYZER_DUMP GENERATOR_TREE ai/commands-upstream.txt > ai/COVERAGE.md
```

## Commit messages

`area: short description (#NNNN). Principle <N>.`, for example
`cmd(caloutput): reject values the hardware does not offer (#7). Principle VII.`
Areas: `cmd`, `ui`, `ai`, `build`, `layout`, `docs`, `governance`, `upstream`.

### Commit signing
Required on `ai`. Setup for every platform is in
[`COMMIT-SIGNING.md`](COMMIT-SIGNING.md).

## Notes for AI agents

Read [`AGENTS.md`](../AGENTS.md) first: section 1 (keeping a unit safe and
recoverable) and section 2 (autonomous boundaries). The house style for replying
to people on issues and PRs is in
[`.specify/memory/community-response-style.md`](../.specify/memory/community-response-style.md).
