# TODO - Software

- Configure clocks for STM32.
    - OSPI works when sys clock is 100MHz and OSPI prescaler is 20.
    - Want clocks as low as possible to save power
- Test screen code
- Test accelerometer code
- Test eeprom code
- Add M95512 eeprom code
- Add code for flash rom
- Add code for IR
- Add code for serial debug
- Add code for buttons
- Add code for speakers
- Figure out how to get device to act as usb slave

## Further code optimisations

- Check accelerometer power modes, sample rate etc. Can definitely save power there <https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bma400-ds000.pdf>
- 25LC512 has a deep power down, might not be worth though
