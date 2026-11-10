# Security Policy

## Reporting a vulnerability

If you discover a security or safety issue in TinySAAI, report it **privately**
rather than opening a public issue: through
[GitHub's private vulnerability reporting](https://github.com/aethersdr/TinySAAI/security/advisories/new)
or a private message to the repository owner.

Include a description, steps to reproduce (the console transcript, the unit,
hardware version and `version` string), and the potential impact. We will
acknowledge your report within 72 hours and work with you on a fix before any
public disclosure.

Problems in the **official** tinySA firmware belong with its authors
([erikkaashoek/tinySA](https://github.com/erikkaashoek/tinySA)).

## Scope

TinySAAI is firmware for a USB-connected instrument that software agents drive.
Security and safety concerns include:

- **Console input handling**: a command line or argument that overflows a
  buffer, crashes or hangs the firmware, or leaves the instrument in an
  undefined or half-changed state (Principle VII)
- **Guard bypass**: any input that reaches a refused item, or changes stored
  calibration or configuration without `confirm` (Principle XII)
- **Recovery**: any path that could make DFU unreachable, write flash outside
  the image and the configuration save, or corrupt the stored calibration
  (Principle III)
- **Generator output**: any path that drives the generator beyond its documented
  limits (Principle VI)
- **Supply chain**: the ChibiOS submodule pin, the toolchain, and the integrity
  of distributed images
- **Host tools** (`ai/*.py`): handling of instrument replies and files

## Supported versions

Only the latest commit on the `ai` branch receives fixes.
