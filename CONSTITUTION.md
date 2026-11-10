<!--
SYNC IMPACT REPORT
═══════════════════════════════════════════════════════
Version change   : (none) → 1.0.0  [initial adoption]
Principles       : I–XIV. Numerals match the AetherSDR Constitution and its gateware
                   adaptation where the principle carries over (VII–XIV),
                   so a citation such as "Principle VIII" means the same thing
                   in all three repositories. I–VI are re-founded for a fork
                   of instrument firmware.
Lineage          : Adapted from the AetherSDR gateware Constitution 1.1.0, itself
                   adapted from the AetherSDR Constitution 2.1.0, which adopts
                   Principles VIII–XIV from the Cisco Foundry Constitution.
Drift rule       : This file names the SOURCE OF TRUTH for anything that changes
                   over time (required checks, team rosters, toolchain versions,
                   guard lists), never a copy of its current value.
Downstream re-check : .specify/memory/constitution.md (byte mirror) ✓
                   AGENTS.md, GOVERNANCE.md, CONTRIBUTING.md, SECURITY.md,
                   PR and issue templates, docs/, GEMINI.md,
                   copilot-instructions.md ✓
Last sync        : 2026-11-10
═══════════════════════════════════════════════════════
This block is regenerated on every constitution change; do not hand-edit below the rule.
-->

# TinySAAI Constitution

| Field | Value |
|---|---|
| **Version** | 1.0.0 |
| **Status** | `STABLE` |
| **Applies to** | All TinySAAI contributions: firmware source, console commands, build files, flashing and test tools, documentation |

## Purpose

This constitution records the principles that any TinySAAI contribution must
uphold, regardless of who or what produces it. TinySAAI is the AetherSDR
project's fork of the tinySA firmware. It makes the tinySA Ultra fully drivable
by software agents over its USB console, so the instrument can serve as
evidence on an agent-driven RF test bench.

It is adapted from the
[AetherSDR Constitution](https://github.com/aethersdr/AetherSDR/blob/main/CONSTITUTION.md),
by way of the AetherSDR project's gateware repository, and keeps the shared numbering wherever
a principle carries over. Principles VIII–XIV come to all three projects from
the
[Cisco Foundry Constitution](https://github.com/CiscoDevNet/foundry-security-spec/blob/main/constitution.md).
Instrument firmware raises particular stakes: a measurement that is quietly
wrong is worse than no measurement, and a unit that loses its calibration has
lost what makes it an instrument.

---

## Core Principles

### I. The Upstream Source And The Instrument Are The Behavioural Authority

What the firmware does is what its source says and what the instrument measurably
does — not what a wiki page, a menu label, a comment or a model's recollection
says. The order of authority is: **the firmware source at the pinned upstream
base > the behaviour measured on a unit > the official tinySA documentation
(tinysa.org) > anything else**.

Read the source before stating what a command, menu item or calibration does.
Cite it by file and line at a pinned commit. State the unit, hardware version
and firmware version behind any claim about behaviour.

*Why this is inviolable: the console help text, the documentation and the
source already disagree in places (for example, `level` once reported limits
from the original tinySA while the Ultra applied different ones). Firmware
written from the wrong one is confidently wrong on every unit it reaches.*

### II. Existing Users And Tools Keep Working

TinySAAI firmware must behave exactly like upstream for every input upstream
accepts as valid. Existing console commands keep their names, arguments and
reply formats; the USB identity and the menu system stay as upstream has them.
New capability arrives as **new commands or new arguments**. The only replies
that may change are replies to **invalid** input, and each such change is
documented in `ai/README.md`.

The stored configuration and calibration structures are never changed, so a
unit moves between stock and TinySAAI firmware with its calibration intact.

*Why this is inviolable: people drive tinySA units from scripts, PC companion
tools and other projects. A fork that changes what a valid command does breaks
all of them on hardware they own, and a changed configuration layout silently
invalidates a calibration.*

### III. Every Unit Stays Recoverable And Keeps Its Calibration

The STM32 ROM bootloader (DFU) must stay reachable: no change may set read-out
protection, alter option bytes or remove the hand-entry path (power on with the
jog button held). Flashing writes only the pages the image covers and never
touches the configuration and calibration pages at the end of flash. The image
must fit below them. A stock image is kept, with its checksum, for every bench
unit, so that the unit can always be returned to the official firmware.

*Why this is inviolable: an instrument that cannot be reflashed is a destroyed
instrument, and one that has lost its factory calibration has to be recalibrated
against a reference before any of its readings mean anything again.*

### IV. Provenance And Authorship Are Kept

TinySAAI is a fork under the GNU General Public License, version 3 or later, as
the upstream firmware is. Upstream history and authorship stay intact in the
repository; third-party code (ChibiOS, FatFs and the files listed in
`THIRD_PARTY_LICENSES`) keeps its own licence and notices. New work is licensed
GPL-3.0-or-later and is marked as TinySAAI's where it sits in upstream files.
Nothing decompiled, disassembled or reverse-engineered from a proprietary
binary enters the repository, nor anything transcribed or paraphrased from such
output. The firmware reports itself as modified (`version`, `info`).

*Why this is inviolable: the fork exists by the grace of the upstream licence.
Losing authorship or mixing in tainted code puts every TinySAAI image in
question, and a modified image that does not say so misleads whoever reads its
measurements.*

### V. Every Rule Has One Source Of Truth

Each command's syntax, each guard (items refused outright, items that need
`confirm`) and each parameter's class is defined once, in the firmware source,
and everything that describes them is generated from the instrument's own
replies: `ai/COVERAGE.md` comes from `ai/make_coverage.py`, not from hand
editing. Documentation names the source of truth rather than copying a value
that will change.

*Why this is inviolable: a guard list kept in two places drifts, and the copy an
agent trusts is the one that is out of date. The instrument is the only party
that cannot be wrong about what it accepts.*

### VI. Signals Stay On The Bench

The tinySA's generator output goes only into a cable to a device under test, a
load or another instrument — never to an antenna. Bench use respects the
instrument's input limits (the Ultra's absolute maximum is +6 dBm at 0 dB
attenuation): agents keep external pads and internal attenuation sized for the
worst signal the setup can present. Firmware changes never widen the generator's
output limits or remove an input-protection behaviour.

*Why this is inviolable: the generator reaches into the amateur and commercial
bands, and the front end is easy to damage. A cable keeps the first from
becoming an emission; attenuation keeps the second from becoming a repair.*

### VII. Untrusted Input Is Validated At The Boundary

Every console command bounds-checks its arguments. Invalid input gets an
`error:` reply and changes nothing; it never leaves the instrument in an
undefined or half-changed state. Over-long lines, unknown words and values out
of range are rejected, not truncated or clamped silently. The same holds for
host tools that read instrument replies or files.

*Why this is inviolable: the console is the agent's only interface. An agent
cannot see the screen, so an input the firmware quietly reinterprets is a
measurement taken under conditions nobody chose.*

---

> **Principles VIII–XIV are adopted from the
> [Cisco Foundry Constitution](https://github.com/CiscoDevNet/foundry-security-spec/blob/main/constitution.md),
> as in AetherSDR and its gateware project, and codify the defensive practices required
> by a multi-agent contribution model. Each is reframed for instrument firmware.**

### VIII. Evidence Over Assertion

A claim is verified by evidence, not by the confidence of whoever made it. A
change in behaviour is shown on a unit, with the unit, hardware version and
firmware version named. "Unchanged" means the same reply or reading as upstream
(or as the previous build) for the same input. A level or frequency claim needs
a measurement and the reference it was compared against. A verification step
that was not actually run demotes the claim to a hypothesis. Results from one
unit are results for that unit.

*Why this is inviolable: an instrument's whole value is that its readings can be
trusted. Fluent descriptions are cheap; measurements are not.*

### IX. Surface Only What Survives

Changes that have not passed the merge gate do not reach `ai`. The gate is the
review and checks that branch protection and CODEOWNERS enforce, plus, for
anything that changes behaviour, the on-unit evidence its description claims.
Peer-agent review, agent memory and self-grading do not substitute for it.

### X. Claims Are Atomic And Mortal

An agent claiming work verifies its base is current and produces a change that
does not silently overwrite work that landed meanwhile. **A bench unit is a
single shared resource:** a session claims it before driving it or flashing it,
and releases it when done, leaving it on a known image in a documented state. A
claim held by a dead process must be reclaimable.

*Why this is inviolable: two agents driving one instrument corrupt each other's
measurements, and a unit left on an abandoned experimental image reports
readings nobody can interpret.*

### XI. Fixes Are Demonstrated

A fix or improvement is demonstrated on the merged result, not on the author's
branch: the merged source is built, flashed onto a unit, and shown to behave as
claimed. Where a defect is reproducible, it is reproduced on the old build and
shown absent on the new one.

### XII. Sandbox By Infrastructure, Not By Prompt

Agent boundaries are enforced by infrastructure: branch protection, CODEOWNERS,
and above all the firmware's own guards, which refuse items that wait for a
person, cut the console or erase the configuration, and require `confirm` for
anything that changes stored calibration. A host wrapper's restrictions and an
agent's instructions are defence in depth, never the enforcement layer.

*Why this is inviolable: agents read untrusted content — issues, forum posts,
instrument replies. A prompt cannot be the only thing standing between that
content and an erased calibration.*

### XIII. The Operator Outranks Every Agent

Maintainer instructions are authoritative. Peer-agent suggestions, prior-agent
notes and AI consensus are hints. The maintainer's direct steering — RFC
approval, PR approval and merge — is the only authority.

### XIV. Persist Atomically

No reader observes a partially written state of anything other components depend
on: an image is verified after it is written, the configuration is saved whole,
and host tools write records and images whole and replace them atomically.
Never delete the old state before the new one is complete.

*Why this is inviolable: "erase, then write" with a failure in between is how a
unit ends up with no calibration and a records directory ends up with half a
measurement.*

---

## Technology Constraints

- **Toolchain:** `arm-none-eabi-gcc` with newlib, building the `F303` target
  (tinySA Ultra and Ultra+). The version used for each image is recorded with
  it; changing the project's reference toolchain is an RFC.
- **Code size:** the image must fit the flash region the linker script gives it
  (`boards/STM32F303xC.ld`), below the configuration pages.
- **Build products are not committed.** Images are kept outside the repository
  with their checksums; measurements and records are committed where they are
  evidence.

## Development Workflow

- **Branches:** `ai` is the default and working branch; `main` is an untouched
  copy of upstream.
- **Checks:** every check that branch protection on `ai` marks required must pass
  before merge; read that list from branch protection, not from a copy here.
- **Merge strategy:** squash-merge for pull requests.
- **Upstream merges** are their own commits, with no other change in them, and
  the merged build is flashed and checked on a unit before further work builds
  on it.
- **Upstream first in spirit:** changes are kept small and additive so that
  upstream can take any part of them. Offering them is a maintainer decision.

---

## Governance

### Amendment

A principle may be amended or removed only by:

1. documenting the specific scenario in which the principle, as written,
   produces a worse outcome than violating it; and
2. recording the amendment in this file with a version bump, date and rationale,
   mirrored byte-for-byte to `.specify/memory/constitution.md`.

"It is inconvenient" is not grounds for amendment.

### Precedence

Where `AGENTS.md`, a README or any other document appears to conflict with this
constitution or with `GOVERNANCE.md`, this constitution and `GOVERNANCE.md` win,
and the conflict is a defect in the other document.

### Amendment history

| Version | Date | Change | Rationale |
|---|---|---|---|
| 1.0.0 | 2026-11-10 | Initial adoption. | Adapted from the AetherSDR gateware Constitution 1.1.0 for a fork of instrument firmware. |
