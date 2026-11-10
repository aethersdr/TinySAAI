<!--
Thanks for contributing to TinySAAI!

Before opening the PR, read AGENTS.md (section 1, keeping a unit safe and
recoverable, first) and CONTRIBUTING.md. If you're an AI agent, assign yourself
to the originating issue before implementing (Principle X).
-->

## Summary

<!-- What changes and why, in one paragraph. Lead with "Fixes #N" / "Closes #N"
when it applies. -->

## Intent

<!-- What will be true after this merges that is not true now, and anything
deliberately out of scope. The diff is judged against this first. -->

## Constitution principle honored

<!-- Cite by Roman numeral, e.g. "Principle VII — attenuate rejects values out
of range" or "Principle VIII — console transcript from a unit attached". -->

## Unit and build

- Unit (model, hardware version):
- `version` string of the image tested:
- Compiler (`arm-none-eabi-gcc --version`):

## Evidence (Principle VIII)

- [ ] **Console:** transcript from the unit for the new or changed commands, valid and invalid input
- [ ] **Unchanged behaviour:** image byte-identical to before with the same `GIT_VERSION`, or the differences explained
- [ ] **Measurements:** level or frequency claims compared against a stated reference
- [ ] **`ai/COVERAGE.md`** regenerated if menus, parameters or commands changed
- [ ] N/A — documentation or tooling only

## Safety

- [ ] **Valid input behaves as upstream**; only replies to invalid input changed, and those are listed in `ai/README.md` — or an approved RFC is linked (Principle II)
- [ ] **No stored structure changed** (`config_t`, saved settings, presets) (Principle II)
- [ ] **No guard weakened**: nothing refused became reachable; nothing that changes stored calibration runs without `confirm` (Principle XII)
- [ ] **DFU stays reachable and the image fits below the configuration pages** (Principle III)
- [ ] **Generator output limits unchanged** (Principle VI)

## Checklist

- [ ] Commits are signed (`docs/COMMIT-SIGNING.md`)
- [ ] TinySAAI changes in upstream files are marked `TinySAAI:` and kept small
- [ ] Provenance: nothing decompiled, disassembled or reverse-engineered from a proprietary binary; third-party notices kept (Principle IV)
- [ ] No build products committed
- [ ] Documentation updated where behaviour or procedure changed. **Not `CHANGELOG.md`**, which is a release-prep file
- [ ] Security- or safety-sensitive changes reference a private advisory where applicable
