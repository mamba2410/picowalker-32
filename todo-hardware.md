# TODO - Hardware

- Finalise power circuitry
    - Add battery model
    - Check battery charging, choose charging chip and signals
        - three signals from charge chip, all three form a 3-bit status code, nearly
    - Decide on 1.8V vs 3.3V (leaning towards 1.8V)
        - Seems like we need 3.3V for some things like IR. Can't just use Vbat in case charging circuit is active
            - IR needs 2.4-5.5V operating
            - Screen VCI and TP_VCC needs 2.7-3.5V
        - be careful when ordering m95512 M95512-D*W* is 2.5-5.5V, M95512-D*R* is 1.7-5.5V, *F* is 1.8-5.5V (MN is SO8, DW is WSSOP8)
            £0.552 version on mouser will do (M95512-DFMN)
            CHECK VERSION ON SCHEMATIC
- Do screen circuitry
    - What's required on the pinouts?
    - Are we using touch screen? A: wire it in, but screen has a lens on so it won't be used
    - Check screen voltages
        - VBAT says "3.8V typ, (2.9-4.5V)"
        - VCI (analogue voltage for driver) says "(2.7-3.5V)"
        - VDDIO/IOVCC 1.72-3.4V
        - TP_VCC 2.7-3.4V
- Does flash rom use qspi or normal spi?
- Finalise flash chip, both come in SOIC8 208/VSOP8 208, both are pin-compatible. Have a 0-ohm resistor/bridge to swap power supply?
    - IS25LP128J(B|F)xx - 2.3-3.6V, £1.88/unit, £1.69/10 unit ($2.30/2.07)
    - IS25WP128J(B|F)xx - 1.65-1.95V, £2.03/unit, £1.83/10 unit ($2.50/2.25)
    - W25Q128JV(S|T)x - 2.7-3.6V, £1.38/unit, £1.22/10 unit ($1.72/1.53)
- Finalise MCU pinouts (nucleo pinouts are different)
    - Do we need external crystal? If so, do we use `OSC32` (PC14,15) or `OSC` (PH0,1)
    - Verify there are no restrictions on interrupt input pins
    - Finalise battery signals (bat sense, power good, bat lo, etc)
- Finalise IR transceiver
    - TFDU4101 is larger
    - TFDU4301 is smaller, slightly cheaper in quantity on mouser uk
- IR shutdown needs signal, idle power draw is 110uA
- u535 datasheet says VDDUSB needs to be min 3.0V (max 3.6V) if usb is used
- Layout PCB

## Breadboard prototyping

SMD components breadboard breakout

- IR transceiver has 0.5mm wide pins, 1mm apart
- M95512 has 0.380mm pins, 1.27mm apart, min horizontal separation 3.9mm, max 6.0mm, SO8N package
- Both flash chips are 8-pin SOIC 208-mil
