# Contributing to TinySAAI

Thanks for your interest. TinySAAI is the AetherSDR project's fork of the tinySA
firmware, making the tinySA Ultra drivable by software agents over its USB
console.

This file is the **policy** side of contributing: what we accept, who reviews
what, and the ground rules. The **operational** side (toolchain, building,
flashing, adding a console command) lives in
[`docs/DEVELOPER-GUIDE.md`](docs/DEVELOPER-GUIDE.md). Roles, decision-making and
the RFC process are in [GOVERNANCE.md](GOVERNANCE.md).

**Problems with the official tinySA firmware** belong with its authors, at
[erikkaashoek/tinySA](https://github.com/erikkaashoek/tinySA) and the
[tinySA forum](https://groups.io/g/tinysa), not here. If you are unsure, check
whether the problem also occurs on the official firmware.

---

## Before you start

1. **Read the [Constitution](CONSTITUTION.md).** (Byte-identical copy:
   [`.specify/memory/constitution.md`](.specify/memory/constitution.md).) Commit
   messages cite the principle a change honours, e.g. `Principle VII.` for a new
   `error:` reply, or `Principle VIII.` for a change backed by measurement.
2. **Read [`AGENTS.md`](AGENTS.md)** if you or your AI tool are new here.
3. **Check whether your change needs an RFC**:
   [GOVERNANCE.md](GOVERNANCE.md#rfc-process). Anything that changes what valid
   input does, a stored structure, a guard, the flashing path, the toolchain or
   a calibration algorithm does.

## Submitting work

1. Create a branch from `ai` (the default branch; `main` is an untouched copy of
   upstream). One change per PR.
2. **Keep it small and additive.** New code goes in `ai/` where it can; changes
   inside upstream files are marked `TinySAAI:` and kept minimal.
3. **Bring evidence** (Principle VIII):
   - new or changed commands → the console transcript from a unit, with the
     unit, hardware version and `version` string;
   - code-only changes → the image byte-identical to before (same
     `GIT_VERSION`), or the differences explained;
   - level or frequency behaviour → a measurement and its reference.
4. **Regenerate `ai/COVERAGE.md`** if menus, parameters or commands changed.
5. **Sign your commits**; see [`docs/COMMIT-SIGNING.md`](docs/COMMIT-SIGNING.md).
6. Open a pull request against `ai` using the template.

---

## Reviews and merging

Review is divided into three tiers by [`.github/CODEOWNERS`](.github/CODEOWNERS).
Self-approval is blocked by GitHub on every tier. What is enforced today is in
[GOVERNANCE.md §Enforcement](GOVERNANCE.md#enforcement).

| Tier | Paths | Who can approve |
|---|---|---|
| **Work (Tier 3)** | `src/`, `ai/` (except `ai/flash.py`), `boards/` (except the linker scripts), `tools/`, `docs/`, and anything not listed below | `@aethersdr/reviewers` |
| **Infrastructure (Tier 2)** | `*.md` outside `docs/` and `ai/` (including the AI-instruction files `AGENTS.md`, `CLAUDE.md`, `GEMINI.md`, `.github/copilot-instructions.md`), `.github/` other than workflows, `THIRD_PARTY_LICENSES`, `Makefile` | `@aethersdr/infrastructure` |
| **Governance, recovery and supply chain (Tier 1)** | The Constitution (both copies), `GOVERNANCE.md`, `CONTRIBUTING.md`, `CODE_OF_CONDUCT.md`, `LICENSE`, `SECURITY*`, `.github/CODEOWNERS`, every workflow, `src/flash.c`, `src/fw_update.*`, `boards/*.ld`, `ai/flash.py`, `.gitmodules`, `lib/ChibiOS` | `@aethersdr/maintainers` |

The flash-writing code, the linker scripts and `ai/flash.py` are Tier 1 because
a mistake there can cost a unit its calibration or its ability to be reflashed
(Principle III). `.gitmodules` and the `lib/ChibiOS` pointer are Tier 1 because
they decide which upstream code the firmware is built from.

Team rosters live in the GitHub org. Bot-opened PRs always need a human
reviewer; no bot is a code owner. Mechanics around review are in
[`docs/PR-WORKFLOW.md`](docs/PR-WORKFLOW.md).

---

## What we will not accept

- **Changes to what valid input does**, or to stored structures, without an
  approved RFC (Principle II).
- **Anything that can make DFU unreachable**, write flash outside the image and
  the configuration save, or cost a unit its calibration (Principle III).
- **A weakened guard**: a refused item made reachable, or a calibration change
  that runs without `confirm`, without an approved RFC (Principle XII).
- **Silent clamping or reinterpretation of invalid input** in new code
  (Principle VII).
- **Behaviour claims without evidence from a unit** (Principle VIII).
- **Anything decompiled, disassembled or reverse-engineered** from a proprietary
  binary, or third-party code without its licence (Principle IV).
- **Committed build products**: `.bin`, `.hex`, `.elf`, `build/`.

---

## Notes for AI agents

Read [`AGENTS.md`](AGENTS.md) first. It is documentation, not policy: where it
appears to conflict with [`CONSTITUTION.md`](CONSTITUTION.md) or
[`GOVERNANCE.md`](GOVERNANCE.md), those win. The limits on what an automated
agent may do without a human deciding are defined in
[GOVERNANCE.md §AI Contributors](GOVERNANCE.md#ai-contributors).

---

## Code of Conduct

See [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md). Ham radio has a long tradition of
helping each other learn; bring that spirit here.

73 de KK7GWY

## License

By contributing, you agree that your contributions are licensed under the
[GNU General Public License, version 3 or (at your option) any later
version](LICENSE), as the upstream firmware is. Third-party code keeps its own
licence (see `THIRD_PARTY_LICENSES`).
