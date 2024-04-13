# TODO - Hardware

- Finalise power circuitry
    - Add battery model
    - Check battery charging, choose charging chip and signals
        - three signals from charge chip, all three form a 3-bit status code, nearly
    - Decide on 1.8V vs 3.3V (leaning towards 1.8V)
        - Seems like we need 3.3V for some things like IR. Can't just use Vbat in case charging circuit is active
- Do screen circuitry
    - What's required on the pinouts?
    - Are we using touch screen?
    - Check screen voltages
- Does flash rom use qspi or normal spi?
- Finalise MCU pinouts (nucleo pinouts are different)
    - Do we need external crystal? If so, do we use `OSC32` (PC14,15) or `OSC` (PH0,1)
    - Verify there are no restrictions on interrupt input pins
    - Finalise battery signals (bat sense, power good, bat lo, etc)
- Do we need IR shutdown? there are just enough pins for it. Depends on power consumption
- Layout PCB
