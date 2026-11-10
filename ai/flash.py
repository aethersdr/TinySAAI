#!/usr/bin/env python3
"""Flash a tinySA Ultra hands-free: 'reset dfu' over the console, dfu-util,
then wait for the console to come back and print the running version.

    python3 ai/flash.py build/tinySA4.bin [--port /dev/serial/by-id/...]

If the unit does not answer on the console, put it in DFU by hand (power off,
hold the jog button, power on) and run with --already-dfu. The STM32 ROM
bootloader is always reachable that way, so a bad image cannot brick the unit.
Flashing erases only the pages the image covers; the stored configuration
(calibration) in the last 16 KB of flash is not touched.
"""
import argparse
import glob
import hashlib
import os
import select
import subprocess
import sys
import time
import tty

DFU_ID = "0483:df11"
PORT_GLOB = "/dev/serial/by-id/usb-tinysa.org_tinySA4_*-if00"
CODE_LIMIT = 0x3C000                       # flash0 length in boards/STM32F303xC.ld


def console(port, line, timeout=3.0):
    fd = os.open(port, os.O_RDWR | os.O_NOCTTY)
    try:
        tty.setraw(fd)
        os.write(fd, (line + "\r").encode())
        out, end = b"", time.time() + timeout
        while time.time() < end and not out.endswith(b"ch> "):
            if select.select([fd], [], [], 0.2)[0]:
                try:
                    out += os.read(fd, 4096)
                except OSError:
                    break                  # the device left USB (reset)
        return out.decode(errors="replace")
    finally:
        os.close(fd)


def wait_for(predicate, timeout, what):
    end = time.time() + timeout
    while time.time() < end:
        if predicate():
            return
        time.sleep(0.5)
    sys.exit("flash: timed out waiting for %s" % what)


def dfu_present():
    out = subprocess.run(["dfu-util", "-l"], capture_output=True, text=True).stdout
    return ("[%s]" % DFU_ID) in out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("image")
    ap.add_argument("--port")
    ap.add_argument("--already-dfu", action="store_true")
    args = ap.parse_args()

    data = open(args.image, "rb").read()
    if not 0 < len(data) <= CODE_LIMIT:
        sys.exit("flash: image is %d bytes; the code region holds %d" % (len(data), CODE_LIMIT))
    sp, reset = int.from_bytes(data[0:4], "little"), int.from_bytes(data[4:8], "little")
    if not (0x20000000 <= sp <= 0x2000A000 and 0x08000000 <= reset < 0x08000000 + CODE_LIMIT):
        sys.exit("flash: %s does not start with a valid vector table" % args.image)
    print("image %s, %d bytes, sha256 %s" % (args.image, len(data), hashlib.sha256(data).hexdigest()))

    if not args.already_dfu:
        ports = [args.port] if args.port else glob.glob(PORT_GLOB)
        if len(ports) != 1:
            sys.exit("flash: need exactly one tinySA console, found %s" % ports)
        print(console(ports[0], "reset dfu").strip().splitlines()[-1:])
    wait_for(dfu_present, 20, "the DFU device")

    subprocess.run(["dfu-util", "-d", DFU_ID, "-a", "0", "-s", "0x08000000:leave",
                    "-D", args.image], check=True, stdout=subprocess.DEVNULL)
    print("flashed")
    wait_for(lambda: len(glob.glob(PORT_GLOB)) == 1, 30, "the console to return")
    time.sleep(1.0)
    port = glob.glob(PORT_GLOB)[0]
    console(port, "", 2.0)
    print(console(port, "version").replace("\r", "").strip())


if __name__ == "__main__":
    main()
