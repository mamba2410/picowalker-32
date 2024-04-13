# Developing the picowalker-32

This document contains materials and instructions to help with developing this project.

## Hardware

List of hardware I'm currently using to prototype on a breadboard:

- [NUCLEO-U545RE Nucleo-64 development board](https://www.st.com/en/evaluation-tools/nucleo-u545re-q.html)
- [dwo DO0180PFST05 AMOLED screen](https://www.dwo.net.cn/pd.jsp?fromColId=2&id=11924#_pp=2_2452)
- [Sparkfun BMA400 breakout board](https://www.sparkfun.com/products/21208) (in SPI mode)
- [Mikroe IrDA 3-click Serial-to-IrDA breakout board](https://www.mikroe.com/irda-3-click)
    - Note: any normal IR transceiver will work when directly driven by the STM32
- [Huawei P8 phone speaker](https://www.aliexpress.us/item/2251832745770085.html)
    - Note: this part isn't finalised until we can get it working
- Some SPI EEPROM. Note: this may be removed in the future
    - [M95512 64k (SMD)](https://www.mouser.co.uk/ProductDetail/STMicroelectronics/M95512-DWDW4TP-K?qs=gNDSiZmRJS8kMWQ2whknqA%3D%3D)
    - [25LC512 64k (SMD, breadboard)](https://ww1.microchip.com/downloads/aemDocuments/documents/MPD/ProductDocuments/DataSheets/25LC512-512-Kbit-SPI-Bus-Serial-EEPROM-Data-Sheet-20002065.pdf)
- Some SPI/QSPI 128Mbit flash
    - [IS25WP128 (1.8V, SMD)](https://www.mouser.co.uk/ProductDetail/ISSI/IS25WP128F-JBLE?qs=W0yvOO0ixfF8aDjNpplUZA%3D%3D)

## Software

All software development (except CubeMX stuff) has been done on linux, I don't know how to build and debug on 
Windows, sorry.

- [KiCad 7.0](https://www.kicad.org/) for the schematics and PCB layouts (haven't tested with 8.0).
- [SamacSys Library Loader](https://www.samacsys.com/library-loader/) for KiCad models and schematics.
- [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html) for pinout and STM32 MCU initialisation and HAL.
- Neovim/VSCode for all other programming.
- CMake/GNU make for building.
- Linux/WSL for compiling and debugging the code.
- [stlink](https://github.com/stlink-org/stlink) and gdb for on-chip debugging.

## Guide

Guide is yet to come.
