# TinySAAI

A fork of Erik Kaashoek's [tinySA firmware](https://github.com/erikkaashoek/tinySA)
that makes the tinySA Ultra drivable by software agents over its USB console.
Everything an agent needs is a console command with a plain, parseable reply.
Nothing requires the touch screen or reading the display.

**This is modified firmware** (GPL-3.0-or-later, as the original). It reports
itself as modified in `version` (suffix `-TinySAAI`) and in `info`
(`Modified: TinySAAI …`). The changes are kept small and additive, so they can be
offered back upstream.

Base: upstream `ab5ecd5` (`tinySA4_v1.4-302-gab5ecd5`), branch `ai`.

## What it adds

| Command | Reply | Notes |
|---|---|---|
| `calibrate` | `calibrate ok`, or `calibrate failed: signal level incorrect at <stage> (<level> dBm); check the cable from CAL to RF`, or `calibrate failed: test failed at <stage> (<level> dBm)`, or `calibrate timeout` | Runs CONFIG > LEVEL CAL > CALIBRATE and blocks until it ends (about 80 s on an Ultra+ ZS407). Needs a short cable from CAL to RF. No "Touch screen to continue" wait. Like the menu version, it leaves the instrument with reset settings and the sweep paused. |
| `state` | One `key value` line per setting | `mode`, `sweep`, `start_hz`, `stop_hz`, `points`, `rbw_hz` (0 = auto), `rbw_actual_hz`, `attenuation_db`, `attenuation_auto`, `lna`, `spur`, `ext_gain_db`, `output`, `generator_hz`, `level_dbm`, `level_min_dbm`, `level_max_dbm` (the last four in output modes), `caloutput`, `correction`, `calibrated`, `last_calibration`. Answers even during a calibration. |

### Every menu item, parameter and measurement

[`COVERAGE.md`](COVERAGE.md) lists everything, generated from the instrument by
`ai/make_coverage.py`: 411 analyzer and 191 generator menu items, 60 numeric
parameters, 16 measurements and 112 console commands, and what is not reachable
from the console (refused items, and the few features not built yet).

| Command | Use |
|---|---|
| `ui` / `ui ls <key>` / `ui tree` / `ui get <key>` | List menu items as `key\|kind\|state\|value\|label`, rendered exactly as the screen renders them, so `state` (on/off) and `value` are live readback. Keys are the labels: `display.waterfall`, `frequency.rbw.rbw_4`, `level.trigger.single`. In a generator mode the root is the generator menu. Listing calls no action. |
| `ui <key> [values...] [confirm]` | Press an item, exactly as a touch would. Values answer the keypad prompts it opens, in order (`ui frequency.span 1M`). Replies `ok` and the item's new line, or `error: … needs a value for "<prompt>"`. |
| `param` / `param <name> [<value> [confirm]]` | All 60 keypad parameters (`span`, `reflevel`, `sweep_time`, `marker`, …) by name: read, or set through the firmware's own keypad handling. |
| `measure` / `measure <name> [values...]` | Set up any of the 16 MEASURE modes with its prompts answered (`measure phase_noise 30M 10k`, `measure oip3 29.99M 30.01M`). A missing value is reported by its prompt text and the measurement is switched off; settings its setup already changed (center, span, RBW, markers) stay changed. `nf_store` saves to flash and needs `confirm`. |
| `readout` | The marker and measurement results, as the screen shows them (`SNR: 54.7dB`, `-91.6dBc/Hz`). Run `waitscan 2` after a change first. |
| `selftest report` | The full self test (CONFIG > SELF TEST) with no touch waits; prints `test <n> pass\|fail\|critical\|not_run <cause>` for each step and a summary. Needs the cable from CAL to RF. |
| `band [n [off]]`, `limit [n [off]]` | The band table and the selected trace's limit lines: list, select and enable, or disable an entry, then set it with `param band_start`, `band_end`, `band_name`, … or `limit_freq`, `limit_level`. |

Guards: items that wait for a person at the screen (touch calibration, self test,
version, brightness, SD browser, USB disk, firmware update, the menu CALIBRATE
items), cut the USB console (connection, serial speed) or erase the configuration
(clear config) are refused. Items and parameters that change the stored
configuration, calibration or synthesizer setup need a trailing `confirm`. Each
press on the way to an item must open the expected menu (INTERNALS opens only with
its code), or `ui` stops before pressing anything inside it.
`state` also reports `measurement`, `unit`, `waterfall` and `level_meter`.

The console line limits are raised from 4 arguments and 48 characters to 10 and 96.

## What it changes

Only replies to **invalid** input change. Valid input behaves exactly as upstream.

| Command | Upstream | TinySAAI |
|---|---|---|
| `attenuate 40`, `attenuate abc` | Accepted (range check commented out; `abc` reads as 0) | `error: attenuate 0..31\|auto`, nothing changes |
| `caloutput 20` | Ignored silently | `error: caloutput off\|30\|15\|10\|4\|3\|2\|1` |
| `level -10` in low output | Clamped silently to the limit | `error: level -115.0..-18.5`, nothing changes |
| `level` (no argument) in low output | `usage: level -76..-6`: fixed text from the original tinySA, wrong on the Ultra | `usage: level -115.0..-18.5`: the limits in use |

## Build and flash

```sh
git submodule update --init
make TARGET="F303"                     # tinySA Ultra / Ultra+
python3 ai/flash.py build/tinySA4.bin  # 'reset dfu' over the console, dfu-util, wait, print version
```

`ai/flash.py` needs `dfu-util` and access to the serial port and DFU device.
The STM32 ROM bootloader is always reachable by hand (power off, hold the jog
button, power on), so a bad image cannot brick the unit: run
`ai/flash.py --already-dfu <image>` from there, or flash the stock image to go
back. Flashing erases only the pages the image covers. The stored configuration
and calibration in the last 16 KB of flash are kept, and no change here alters
the stored config structures, so the unit keeps its calibration across the stock
and TinySAAI firmware.

## Verification

On a tinySA Ultra+ ZS407 (hardware V0.5.4), 2026-11-10:

- Unmodified upstream `ab5ecd5` built with Arch `arm-none-eabi-gcc` 16.2
  (upstream releases use GCC 11.3) and flashed: CAL self-check identical to
  the stock image (−35.44 dBm, 5 of 5 runs each). No new compiler warnings from
  the TinySAAI changes.
- `calibrate` with CAL to RF: `calibrate ok` in 78 s; the level offsets
  changed by ±0.1–1.0 dB, as with the menu calibration.
- With the same stored calibration, the CAL self-check gave −35.0 to −36.0 dBm
  in 0.5 dB steps on both the stock and the TinySAAI firmware (5 runs each,
  twice for TinySAAI). The steps are the instrument's 0.5 dB power-detector
  resolution.
- Every error case in the table above, and the valid cases next to them, on the
  device.

- `ui tree` walked in analyzer and generator mode; no duplicate keys; the stored
  calibration (level offsets, correction tables, frequency correction) identical
  before and after all the tests. Sweep-thread stack: 528 bytes free at the
  worst point seen (`threads`).
- `param` and `ui` reads, presses with keypad values, refusals and `confirm`
  guards; `measure` with and without enough values; `readout` after `waitscan`
  agreeing with `marker` on the CAL tone (phase noise −91.6 dBc/Hz at 10 kHz,
  channel power, SNR 54.7 dB, harmonics of the square-wave CAL output); 15 of
  the 16 measurement modes set up and read (`nf_store` not run).
- Screen captures after keypad presses in analyzer and generator mode: the
  keypad closes as after a touch entry.

Not yet tested: the `calibrate` failure replies (it needs the cable removed),
the timeout, and pressing every one of the 600 items (all were listed; a
representative set of each kind was pressed: settings, values, actions,
children of callback-opened menus, generator items, INTERNALS).

## Backlog

- `calibrate harmonic` (above 5.34 / 7.37 GHz, needs an external source).
- `selftest` results as text instead of only on screen.
- Machine-readable errors for the remaining commands that accept bad input
  silently.
