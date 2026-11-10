# PR Workflow

How changes move through review and land on `ai`. *Who* approves *what* is
policy, in [`CONTRIBUTING.md`](../CONTRIBUTING.md#reviews-and-merging) and
[`.github/CODEOWNERS`](../.github/CODEOWNERS); this covers the mechanics.

## Drafts

- **Human-authored draft:** work in progress; reviewers skip it until it is
  marked ready.
- **Bot-authored draft:** awaiting human review; treat it as ready for triage.

## Evidence before review

A PR that changes behaviour is not ready until its evidence is attached: the
console transcript from a unit, the unit and `version` string, and for code-only
changes the byte comparison against the previous build. Reviewers read the
Intent and Evidence sections of the template before the diff.

## Stale branches

Branches need not be up to date with `ai` before merging; squash-merge runs a
fresh three-way merge. A PR whose evidence was produced against an older base
must say which base, and anything that changes behaviour is rebuilt and checked
on a unit from the merged result (Principle XI).

## Upstream merges

Merging from upstream is a maintainer decision. It is its own commit with no
other change in it. Files upstream added in the root are moved into the fork's
layout in a follow-up commit. The merged build is flashed onto a unit, its
stored calibration compared with the record, and the CAL self-check run before
other work builds on it.

## Recovering from a broken `ai`

1. Identify the offending commit.
2. **Fix forward** if the change is small: a normal PR titled
   `fix: repair ai after <SHA>`.
3. **Revert** otherwise: `git revert <sha>` on a branch, then a PR. Never
   force-push `ai`.
4. If a bench unit was left on a broken image, return it to the last good image
   (or stock) before anything else, and record it in the PR.
