# Proteus ZMK Config (with dongle)

> This firmware repository is a fork of the original project found at [aroum/zmk-enki42-dongle](https://github.com/aroum/zmk-enki42-dongle). A huge thank you to the original creators, their excellent work is the basis for the repository and the current firmware is largely **exactly the same**.

The config allows you to use 3 n!n, one for the dongle, one for the left half, one for the right half. Using a dongle significantly increases battery life. The dongle can be any device on the nRF52840. 

Check out [ZMK Power Profiler](https://zmk.dev/power-profiler) to see the effect a dongle can have. In a dongle setup, the dongle is the central and both your halves are peripherals!

The default config contains my own personal daily-driver layout but feel free to change it out for your own :)

This is a config for **Proteus**, but you can use it with corne or any other compatible keyboard.

## Changes
- Added **settings_reset** to `build.yaml`
- Changed the **board name** to `Proteus`
- Disabled **sleep** for both halves
- Changed the **keymap**

## Install

Before flashing this firmware, flash the [setting reset firmware](https://zmk.dev/docs/troubleshooting#split-keyboard-halves-unable-to-pair) into all 3 controllers. Then flash this firmware on 3 devices. If the halves do not connect themselves, try pressing the reset buttons on the dongle and keyboards.

## Credit

* [@aroum](https://github.com/aroum/zmk-enki42-dongle) entirely for all the firmware contained!
