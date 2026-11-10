# GitHub Copilot Instructions for TinySAAI

**Canonical project guide: [`AGENTS.md`](../AGENTS.md).** Read it before suggesting
or writing anything. This file holds only the must-knows.

## Must-knows

1. **Never send `confirm`** for a change to stored calibration or configuration
   unless the maintainer asked for that exact change. The firmware's refusal
   without it is the guard (Principle XII).
2. **Never change a stored structure** (`config_t`, saved settings); a unit must
   keep its calibration across stock and TinySAAI firmware (Principles II, III).
3. **Valid input behaves exactly as upstream** (Principle II). New capability is
   a new command or argument; only replies to invalid input may change.
4. **Keep DFU reachable and the stock image archived** before flashing a unit
   (Principle III).
5. **The TinySAAI Constitution governs every contribution**: see
   `CONSTITUTION.md` (byte mirror: `.specify/memory/constitution.md`). Commit
   format: `area: short description (#NNNN). Principle <N>.`
6. **Evidence over assertion** (Principle VIII): name the unit, hardware version
   and `version` string behind every behaviour claim.
7. **Signed commits required on `ai`**, the default branch; `main` is an
   untouched copy of upstream. See `docs/COMMIT-SIGNING.md`.
8. **Claim before you act** (Principle X): assign yourself before implementing,
   and claim a bench unit before driving or flashing it.
