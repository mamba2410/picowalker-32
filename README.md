# picowalker-32

Thsi repo contains all the designs, schematics, pcb layouts and code for the *picowalker-32* project.

The *picowalker-32* is a modern, open source and faithful recreation of the *Pokewalker* from *Pokemon HeartGold/SoulSilver*.
The goal is to have a device that fully mimics the features and functionality of a *Pokewalker*, except made with modern 
features such as a rechargeable battery, colour screen and an additional flash memory chip to store more data.

The *picowalker-32* is based on the STM32U535 chip, which has far more ram, flash and computing capabilities than the original
processor. Most importantly, its available to purchase, even in small quantities.
Due to the lack of options for screens and batteries, the *picowalker-32* is slightly larger than the original *Pokewalker*.

## What's in this repo

- Design files for the schematics and PCB layout, drawn in KiCAD 7.
- STM32CubeMX configuration for the STM32U545 chip used in the design.
- STM32 code using the STM32 HAL for the drivers and dependencies that the [picowalker-core](https://github.com/mamba2410/picowalker-core) needs to function.
- Developer notes and a bill of materials used in testing.
- Other, miscellaneous notes related to the *picowalker-32* project.

## The state of the project

Currently, the project is in active development.
There are schematics, the start of some code and materials that have been picked out.
The project is being prototyped on a breadboard using the *NUCLEO-U545RE* development board and other
breakout boards for fast prototyping.

Left to do:

- Finish the schematics (power delivery, other checks).
- Finish the driver for the screen.
- Start drivers for the IR, accelerometer, eeprom, flash memory, sound, buttons and others.
- Route the PCB (either a test PCB or directly onto the final form factor).
- Manufacture and test the PCB and iterate on it.

## Development and Contribution

If you want to help develop this project, make sure to read the [development document](./development.md) for instructions 
on how to recreate the project at home.

I encourage you to come join the [Discord]()! This is where most of the discussion happens about the project.
There's also other stuff related to the *Pokewalker* including reverse enginnering and hacking it.

Thanks to all the contributors who have helped out a lot, especially

- Kamp, for doing all the CAD work for the shells, picking out components like the screen and battery (which were really hard to find!) and others.
- Zenith, for help with sprites and images and even recreating most of the coloured ones to work on the walker.
- PoroCYon, for help with schematics, electrical questions and picking a suitable MCU.

## Licensing

I'm not sure about the license, I want this project to be as open and accessible as possible.

