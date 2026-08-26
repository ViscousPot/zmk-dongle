# Board in the Shell ZMK Config (with dongle)

> This repo is a fork of [aroum/zmk-enki42-dongle](https://github.com/aroum/zmk-enki42-dongle). Huge thanks to the original creators, their work is the basis for this repository.

ZMK config for **Board in the Shell**. Uses 3 nRF52840 controllers: one dongle, one left half, one right half. The dongle is the central and the halves are peripherals, which is much better for battery life. See the [ZMK Power Profiler](https://zmk.dev/power-profiler).

Project homepage: https://bits.viscouspotenti.al/ - model files, full instructions, and the pipeline for building new firmware.

The default keymap is my daily driver layout, swap it out for your own.

## Install

Before flashing this firmware, flash the [settings reset firmware](https://zmk.dev/docs/troubleshooting#split-keyboard-halves-unable-to-pair) into all 3 controllers. Then flash this firmware on 3 devices. If the halves do not connect themselves, try pressing the reset buttons on the dongle and keyboards.

## Regenerating keys

The dongle and halves ship with preshared BLE addresses and long term keys (`config/boards/shields/enki42/proteus_bonds.h`) so they boot already paired and never go through SMP. If you build your own board you should generate your own set so it doesn't collide with anyone else's:

```
python3 scripts/regen_bonds.py
```

This rewrites the addresses and LTKs in the header. Then rebuild, flash the settings reset firmware on all 3 controllers, and flash the new firmware.

## Credit

- [@aroum](https://github.com/aroum/zmk-enki42-dongle) for the original firmware
