# TinySAAI Project Governance

This document describes how TinySAAI is governed: who has authority over what,
how decisions are made, and what an automated agent may and may not do. See
[CONTRIBUTING.md](CONTRIBUTING.md) for the mechanics of submitting work and
[CONSTITUTION.md](CONSTITUTION.md) for the principles every contribution upholds.

TinySAAI is part of the [AetherSDR](https://github.com/aethersdr/AetherSDR)
project and follows the same governance model, adapted for instrument firmware.

---

## Project Direction

TinySAAI's goal is to **make the tinySA Ultra fully drivable by software agents
over its USB console**: every setting, measurement and reading reachable through
a command with a parseable reply, every invalid input answered with an error,
and nothing that waits for a person at the screen. Every technical decision is
evaluated against that goal and against two constraints that override it:

- **Existing users and tools keep working** (Principle II). Valid input behaves
  exactly as upstream, and the stored configuration layout never changes.
- **Small and additive.** Changes are kept so that upstream can take any part of
  them. TinySAAI is not a competing tinySA firmware.

**The project maintainer (Jeremy Fielder / KK7GWY) holds final authority on:**

- Scope: which instrument features get console access, and how
- The command interface: names, arguments, reply formats and guards
- Targets: which tinySA models and hardware versions are supported
- Toolchain: the reference compiler and build options
- Upstream relationships: merging from upstream, offering changes upstream, and
  any contact with the tinySA authors

This is not a committee. When there is disagreement about direction, the
maintainer's decision is final.

---

## Roles

### Contributor
Anyone who opens a PR. No special permissions.

### Triager
Can label, close and comment on issues. Cannot merge.

### Domain Maintainer
Can review and merge PRs **within a designated area**, subject to the RFC and
CODEOWNERS requirements below.

| Area | Path(s) | Notes |
|---|---|---|
| Console commands | `ai/` (except `ai/flash.py`), TinySAAI changes in `src/` | Behaviour changes still need on-unit evidence |
| Documentation | `docs/`, `*.md` | Governance documents excepted |

No domain maintainers are appointed yet. **How to earn it:** three or more
merged PRs in the area with sound evidence. Open an issue titled
`[Governance] Domain Maintainer request: <area>`.

### Project Maintainer
Jeremy Fielder ([@ten9876](https://github.com/ten9876)). Final authority on all
decisions. Assisted by Claude (AI development partner); see AI Contributors.

---

## RFC Process

Some changes need an approved RFC before implementation begins.

### What requires an RFC

- **Changing what valid input does**: an existing command's arguments, reply
  format or effect, a menu item's behaviour, or the USB identity
- **Changing any stored structure**: the configuration, calibration, presets or
  anything else written to flash (`config_t`, `setting_t` saves)
- **Weakening a guard**: making a refused item reachable, or letting an item,
  parameter or measurement that changes stored calibration run without
  `confirm`
- **Changing a calibration or measurement algorithm**
- **Changing the generator's output limits**, or anything that reduces input
  protection (Principle VI)
- **Changing the flashing path**: the linker script's flash regions, `ai/flash.py`,
  the SD-card firmware update, DFU entry, or anything that writes flash outside
  the configuration save
- **Changing the reference toolchain** or build options
- **Adding a hardware target** (another tinySA model or hardware version)
- **New external dependencies** in the firmware or its tools
- **Merging from upstream** and **offering changes upstream** (maintainer
  decision; the RFC records it)

When in doubt, open an RFC issue first and ask.

### What does NOT require an RFC

- New **read-only** console commands, and new replies for queries
- New commands or arguments that reach existing instrument behaviour through
  the existing guards, with refused and `confirm` items handled as the
  existing ones are
- `error:` replies for input that upstream accepted silently but was invalid
  (documented in `ai/README.md`)
- Regenerating `ai/COVERAGE.md`; tests and records from a bench unit
- Documentation additions and corrections
- Build fixes that change no option and no output

### How to write an RFC

Open an issue with the label `rfc` and the title prefix `[RFC]`, using the RFC
template. Describe the problem, the proposal, the effect on existing users and
on stored data, the code-size cost, how it will be shown on a unit, and the
alternatives considered. Do not open a PR until the RFC is approved.

---

## CODEOWNERS

PR review is gated by [`.github/CODEOWNERS`](.github/CODEOWNERS), the
authoritative source of who must approve what (last match wins). It defines
three tiers:

- **Tier 3 — work** (`@aethersdr/reviewers`): `src/`, `ai/` (except
  `ai/flash.py`), `boards/` (except the linker scripts), `tools/`, `docs/`, and
  anything not enumerated below.
- **Tier 2 — infrastructure** (`@aethersdr/infrastructure`): `*.md` outside
  `docs/` and `ai/`, the AI-instruction files, `.github/` other than workflows,
  `THIRD_PARTY_LICENSES`, the `Makefile`.
- **Tier 1 — governance, recovery and supply chain** (`@aethersdr/maintainers`):
  the Constitution (both copies), `GOVERNANCE.md`, `CONTRIBUTING.md`,
  `CODE_OF_CONDUCT.md`, `LICENSE`, `SECURITY*`, `.github/CODEOWNERS`, every
  workflow, **the flash-writing code and flashing path** (`src/flash.c`,
  `src/fw_update.*`, `boards/*.ld`, `ai/flash.py`), and **`.gitmodules` and
  `lib/ChibiOS`** (which upstream code, and which commit of it, the firmware is
  built from).

Tier 1 is reserved for paths where a wrong change alters **who decides**, **whether
a unit stays recoverable and keeps its calibration**, or **what gets built**.
`.github/CODEOWNERS` wins on any disagreement with this summary. Self-approval is
blocked by GitHub on every tier.

---

## Enforcement

Principle XII puts enforcement in infrastructure. For this repository:

| Mechanism | State |
|---|---|
| Org teams with access to the repository (needed for CODEOWNERS): maintainers admin, infrastructure maintain, reviewers write, triage triage | In force since 2026-11-10 |
| Branch protection on `ai`: signed commits, no force-push, no deletion (admins exempt) | In force since 2026-11-10 |
| Required review on `ai` | Not in force: the maintainer pushes directly, so CODEOWNERS applies to pull requests only |
| Private vulnerability reporting | In force since 2026-11-10 |
| Firmware guards (refused items, `confirm`) | In force in every TinySAAI image |

Read the live settings from GitHub; this table can lag them.

---

## AI Contributors

### Automated agents

**The following limits are normative and defined here.** They apply to every
automated agent working in this repository.

- **May autonomously:** build the firmware; flash a bench unit after claiming
  it (Principle X), keeping the stock image and the last good image archived
  with checksums; drive a bench unit through the console, including the
  generator into a cable, load or device under test; run `calibrate` with the
  CAL-to-RF cable fitted; add read-only commands and commands that reach
  existing behaviour through the existing guards; add `error:` replies for
  invalid input; regenerate `ai/COVERAGE.md`; write documentation.
- **May not autonomously:** send `confirm` for anything that changes stored
  calibration or configuration, except a calibration the maintainer asked for;
  change a stored structure; weaken or remove a guard; change what valid input
  does; change the flashing path or anything that could make DFU unreachable;
  widen the generator's output limits; connect the generator to an antenna;
  change the reference toolchain; add a dependency; merge from upstream; or open
  an upstream PR or contact the tinySA authors on the project's behalf.

When in doubt, the agent stops at the evidence and asks the maintainer.

`AGENTS.md` elaborates these limits for agent consumption. It **may not widen
them**; where it differs from this section, this section governs.

Bot-opened PRs are reviewed under the same CODEOWNERS tiers as any other PR. No
bot is a member of a code-owner team, so a bot PR always needs a human approver.

### AI-assisted human contributions

Welcome, and held to the same standards. The human submitting the PR is
responsible for understanding the change, the evidence behind it, and its
effect on the instrument.

---

## Decision Making

For ordinary PRs, a reviewer comments and the maintainer or a Domain Maintainer
merges or requests changes. For significant decisions (RFC-required changes,
roles, changes to this document), the maintainer may open a discussion period of
at least 5 days. Community input is welcome; the maintainer's decision is final.

---

## Amendments

This document may be updated by the project maintainer at any time. Significant
changes are made through a PR labelled `governance`.

---

*73 de KK7GWY*
