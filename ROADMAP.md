# TinySAAI Roadmap

Direction is reserved to the project maintainer
([GOVERNANCE.md](GOVERNANCE.md#project-direction)). This file describes intent,
not commitments. What is done is in [`ai/README.md`](ai/README.md).

## Now: complete console coverage on the tinySA Ultra+

- `calibrate harmonic`: the level calibration above 5.34 / 7.37 GHz, which needs
  an external source.
- `error:` replies for the remaining upstream commands that accept invalid input
  silently.
- Press every menu item from the console at least once on a unit (all are
  listed and a representative set of each kind has been pressed).
- Loading from the SD card and the curve editor, which are not reachable from
  the console yet.

## Next: evidence and reach

- A build check on every push and pull request.
- Level accuracy of the console calibration compared against a better reference
  instrument.
- Other tinySA Ultra models and hardware versions (each is an RFC: a new target).
- The original tinySA (`F072` target), which does not currently link (RAM
  overflow).

## Later: upstream

Offer the parts the upstream authors find useful, in upstream's layout. A
maintainer decision, recorded by RFC.
