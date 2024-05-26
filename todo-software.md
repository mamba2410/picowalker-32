# TODO - Software

- Configure clocks for STM32.
    - OSPI works when sys clock is 128MHz and OSPI prescaler is 16.
    - Want clocks as low as possible to save power
    - Screen drawing is slow when core speed is slow - use DMA
- Test screen code
- Test accelerometer code
- Test eeprom code
    - Add a `delay_us` function because waiting 1ms between spi actions is wasteful
- Add M95512 eeprom code
- Add code for flash rom
    - Just a note: both flash memories return data latched on falling edge, but accept data on rising edge
- Add code for IR
- Add code for serial debug
- Add code for buttons
- Add code for speakers
- Figure out how to get device to act as usb slave
- Sleep mode

## Sleep mode

- Screen sleep - Sleep in mode after 30 sec, deep sleep after another 60 sec (arbitrary numbers)
- controller sleep - sleep mode loop and stop 2/3 after 30 sec ? (check limitations of this mode eg. peripheral access)
- 25LC512 has a deep power down, but chip not used in final version
- is25wp128 has a deep power down, enter when in screen deep sleep
- bma400 auto switches between low (1.2uA) and normal power mode. Sleep mode probably useless since we always want to count steps
- IR has a shutdown signal, set that when comms app is not running
    - Idle current <110uA, standby current <1uA, transmitter draws 200-430mA in use
- m95512 auto sleeps when CSB is high
