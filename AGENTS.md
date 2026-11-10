# Agent guide for TinySAAI

The canonical project guide for every AI tool and every human contributor. It is
**documentation, not policy**: where it appears to conflict with
[`CONSTITUTION.md`](CONSTITUTION.md) or [`GOVERNANCE.md`](GOVERNANCE.md), those
win, and the conflict is a defect in this file. This file may not widen the
agent limits in GOVERNANCE.md.

TinySAAI is a fork of the tinySA firmware that makes the tinySA Ultra drivable
over its USB console. What it adds is described in [`ai/README.md`](ai/README.md);
everything an agent can reach is listed in [`ai/COVERAGE.md`](ai/COVERAGE.md).

## The principles, in one line each

The [Constitution](CONSTITUTION.md) governs every contribution. Cite the
principle a change honours in its commit message (`… Principle VII.`).

- **I** — The firmware source at the pinned base, then measured behaviour, are the authority. Cite file:line.
- **II** — Valid input behaves exactly as upstream; stored structures never change.
- **III** — Every unit stays recoverable (DFU reachable) and keeps its calibration.
- **IV** — GPL-3.0-or-later; upstream authorship kept; nothing from proprietary binaries.
- **V** — One source of truth: guards and syntax in the source, `COVERAGE.md` generated from the instrument.
- **VI** — Signals stay on the bench: generator into a cable, input within its limits.
- **VII** — Validate console input; invalid input gets `error:` and changes nothing.
- **VIII** — Evidence over assertion: name the unit, hardware and firmware version.
- **IX** — Only what survives the merge gate reaches `ai`.
- **X** — Claims are atomic and mortal, including claims on a bench unit.
- **XI** — Fixes are demonstrated on the merged build, flashed onto a unit.
- **XII** — Sandbox by infrastructure: the firmware's guards, not a prompt.
- **XIII** — The operator outranks every agent.
- **XIV** — Persist atomically: images, configuration saves, records.

## 1. Keeping a unit safe and recoverable — read first

- **Never send `confirm`** for an item, parameter or measurement that changes
  stored calibration or configuration, unless the maintainer asked for that
  exact change. The firmware refuses these without it; that refusal is the
  guard.
- **Never change a stored structure** (`config_t`, saved settings, presets).
  The unit's calibration lives in the last 16 KB of flash and must read the same
  on stock and TinySAAI firmware.
- **Keep DFU reachable.** No option-byte, read-out-protection or bootloader
  change. Keep the stock image and the last good image for every bench unit,
  with SHA-256 checksums, before flashing anything new.
- **Check the image size** against the flash region in `boards/STM32F303xC.ld`
  before flashing; `ai/flash.py` refuses an image that would reach the
  configuration pages.
- **Signals stay on the bench.** The generator output goes into a cable to a
  device under test, a load or another instrument, never an antenna. Keep pads
  and attenuation sized for the strongest signal the setup can present (the
  Ultra's absolute maximum input is +6 dBm at 0 dB attenuation).

## 2. Autonomous agent boundaries

These restate [GOVERNANCE.md §AI Contributors](GOVERNANCE.md#ai-contributors),
which is normative.

| You may, autonomously | You may not, without the maintainer |
|---|---|
| Build the firmware | Send `confirm` for a calibration or configuration change you were not asked for |
| Flash a bench unit after claiming it, with the stock and last good images archived | Change a stored structure, or what valid input does |
| Drive a unit over the console, generator into a cable, load or DUT | Weaken or remove a guard (refused items, `confirm`) |
| Run `calibrate` with the CAL-to-RF cable fitted | Change the flashing path, linker regions or anything that could make DFU unreachable |
| Add read-only commands, and commands that reach existing behaviour through the existing guards | Widen the generator's output limits; connect it to an antenna |
| Add `error:` replies for invalid input | Change the reference toolchain, or add a dependency |
| Regenerate `ai/COVERAGE.md`; write documentation | Merge from upstream, open an upstream PR, or contact the tinySA authors |

When in doubt, stop at the evidence and ask.

## 3. Upstream and provenance (Principles I and IV)

- `main` is an untouched copy of upstream
  ([erikkaashoek/tinySA](https://github.com/erikkaashoek/tinySA)); `ai` is the
  fork. The current base is recorded in `ai/README.md`.
- Keep changes **small and additive**. Mark TinySAAI code in upstream files with
  a `TinySAAI:` comment, and put new code in `ai/` where it can live there.
- The fork's layout differs from upstream's (see the README's Layout section).
  A file upstream adds later lands in the root and is moved by hand when the
  merge is done; changes offered back upstream have their paths rewritten.
- Third-party code keeps its licence and notices (`THIRD_PARTY_LICENSES`).
  Nothing decompiled, disassembled or reverse-engineered from a proprietary
  binary.

## 4. Evidence (Principle VIII)

- Name the unit (model, hardware version) and the exact firmware version
  (`version`) behind every behaviour claim.
- "Unchanged" means the same reply or reading as the previous build for the same
  input. Code-only refactors are shown byte-identical where they can be (build
  both with the same `GIT_VERSION=…`; only the embedded build time differs).
- Level claims need a reference: the CAL output self-check, or a comparison
  against a better instrument.
- One unit's result is one unit's result.

## 5. Claims (Principle X)

- **Assign yourself before implementing a change or landing a PR.** Reviews and
  comments take no claim.
- Verify your base is current against `origin/ai` before producing a patch.
- **Claim a bench unit before driving or flashing it**, and release it when
  done, on a known image and in a documented state (parked: generator off,
  attenuation up).

## 6. Builds

- `make TARGET="F303"`; the image is `build/tinySA4.bin`. Procedures and
  conventions are in [`docs/DEVELOPER-GUIDE.md`](docs/DEVELOPER-GUIDE.md).
- Record the compiler version with every image you flash or archive.
- Build products are never committed.

## 7. Commits and PRs

- **Signed commits are required** on `ai`; see
  [`docs/COMMIT-SIGNING.md`](docs/COMMIT-SIGNING.md).
- Message format: `area: short description (#NNNN). Principle <N>.`, for
  example `cmd(level): reject levels outside the active limits (#12). Principle VII.`
  Areas: `cmd`, `ui`, `ai`, `build`, `layout`, `docs`, `governance`, `upstream`.
- **`CHANGELOG.md` is a release-prep file.** Ordinary changes never add to it;
  describe the change in the PR body. `changelog.txt` is upstream's and is
  left as upstream has it.
- Use the PR template; its checklist is the evidence reviewers look for.
