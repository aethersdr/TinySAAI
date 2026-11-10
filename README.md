TinySAAI: tinySA firmware for software agents
==========================================================

**TinySAAI is an independent fork** of Erik Kaashoek's
[tinySA firmware](https://github.com/erikkaashoek/tinySA). The
[AetherSDR](https://github.com/aethersdr/AetherSDR) project maintains it. It is
not the official tinySA firmware, and the tinySA authors do not endorse or
support it. For the official firmware, releases and support, go to
[erikkaashoek/tinySA](https://github.com/erikkaashoek/tinySA) and
[tinysa.org](https://tinysa.org/wiki/).

The fork makes a tinySA Ultra fully drivable over its USB console by software,
and in particular by AI agents. Everything you can do from the screen, an agent
can do with a console command that gives a plain, parseable reply. It doesn't
need to touch the screen or read the display.

- Branch `ai` holds the fork's work, based on upstream `ab5ecd5`
  (`tinySA4_v1.4-302`).
- [`ai/README.md`](ai/README.md) lists the commands, the guards and how each one
  was tested.
- [`ai/COVERAGE.md`](ai/COVERAGE.md) lists every menu item, parameter,
  measurement and command an agent can reach. It is generated from the
  instrument's own replies.

## Why this fork exists

AetherSDR is building gateware for openHPSDR-style radios. Every change is
proven on an RF test bench that AI agents run: they key the radio into a dummy
load and measure what comes out. On that bench the tinySA Ultra is the
instrument agents use most often. To be useful to them it has to be operated
the way software operates things: through an interface, not through a touch
screen.

The upstream firmware already has a good USB console, written for people and
for PC companion tools. Agents hit four gaps in it:

- **Much of the instrument is reachable only from the menus.** This includes
  most of the MEASURE modes (phase noise, SNR, OIP3, channel power, noise
  figure and others), the band and limit tables, the self test and the level
  calibration. An agent could reach them only by simulating touches and reading
  screenshots. That is slow and fragile, and it is not how software should
  drive a device that has a console.
- **The results of those features appear only on the screen.** A measurement an
  agent can't read back is a measurement it can't use as evidence.
- **Some commands accept bad input silently.** For example, an attenuation out
  of range is accepted, and a generator level outside the limits is quietly
  clamped. An agent can't tell from the reply that the instrument did something
  other than what it asked.
- **Some operations wait for a person.** For example, "Touch screen to continue"
  appears at the end of a calibration or self test, with no one there to touch
  it.

TinySAAI closes those gaps and changes nothing else:

- **Everything reachable, everything readable.** The `ui`, `param` and `measure`
  commands reach every menu item, keypad parameter and measurement. `readout`,
  `state`, `selftest report` and `calibrate` return their results as text.
- **Errors you can see.** Invalid input gets an `error:` reply and changes
  nothing. Valid input behaves exactly as upstream.
- **Guards for what matters.** Anything that would change the stored
  calibration, correction tables, synthesizer setup or saved preferences needs
  an explicit trailing `confirm`. The firmware refuses outright anything that
  would wait for a person, cut the USB console or erase the configuration. An
  agent can explore the instrument freely without damaging its calibration.
- **Compatible with stock firmware.** The stored configuration structures are
  unchanged, so a unit keeps its calibration when it moves between the stock
  and TinySAAI firmware.
- **Honest about being modified.** `version` ends in `-TinySAAI`, and `info`
  says `Modified: TinySAAI`.

The changes are small and additive. We keep them that way so that any part the
upstream authors find useful can be offered back to them. Making the tinySA work
well for agents should not require a permanent fork.

## Status

The fork has been built and tested on one tinySA Ultra+ (ZS407, hardware
V0.5.4) only. The other Ultra models and the original tinySA (`F072` target)
have not been built or tested with these changes. [`ai/README.md`](ai/README.md)
records what was verified on the instrument and what was not.

## Build and flash

Get the source with its submodule:

    $ git clone -b ai https://github.com/aethersdr/TinySAAI.git
    $ cd TinySAAI
    $ git submodule update --init --recursive

Build for the tinySA Ultra / Ultra+ with a recent `arm-none-eabi-gcc`:

    $ make TARGET="F303"

Flash without touching the unit (needs `dfu-util` and access to the serial
port). The script switches the unit to DFU over the console, flashes the image,
waits for it to restart and prints the version:

    $ python3 ai/flash.py build/tinySA4.bin

You can always reach the STM32 ROM bootloader by hand: power off, hold the jog
button and power on. A bad image therefore can't brick the unit. From the
bootloader, run `python3 ai/flash.py --already-dfu <image>`, or flash the
official firmware to go back. Flashing keeps the settings and calibration
stored in the last 16 KB of flash.

## Issues

Report problems with the TinySAAI additions in this repository's
[issues](https://github.com/aethersdr/TinySAAI/issues), not to the tinySA
authors or the tinySA groups.io forum. If you're unsure whether a problem comes
from the fork, reproduce it on the official firmware first.

## Licence

GPL-3.0-or-later, the same as the upstream firmware. Upstream's history and
authorship are kept intact in this repository.

## Note

tinySA is a trademark owned by its respective owner. TinySAAI is not a tinySA
product. The name only describes what the firmware runs on.

## Credit

The tinySA firmware is the work of
[@erikkaashoek](https://github.com/erikkaashoek) and its contributors:

* [@edy555](https://github.com/edy555)
* [@hugen79](https://github.com/hugen79)
* [@cho45](https://github.com/cho45)
* [@DiSlord](https://github.com/DiSlord/)

The TinySAAI additions are by the AetherSDR project.
