# Button Interrupt Blink
Cycles through four led blinking states when a button is pressed. The states are shown below:

State Number | LED Function
--- | ---
0 | off
1 | fast blink
2 | medium blink
3 | slow blink

State 0 is entered on startup. To change the LED and button pins, change the #define statements at the top that give generic names for gpio registers and pins.

The program uses TimerA0 to time LED flashes. It is set to up mode, 8x prescaling, and the value of TA0CCR0 (top value) changes depending on the state. An interrupt is generated when the timer overflows, which is caught by an interrupt vector, which toggles the LED pin.

The button press is detected using an interrupt as well. The interrupt is configured to generate on a rising edge, which corresponds to the press (not release) of the button. In the button's interrupt vector, the current state is updated, then TA0CCR0 is updated to its new value based on the new state.

### Differences Between Implementation for each Processor
- The FR6989 needs the line `PM5CTL0 &= ~LOCKLPM5`, which disables the GPIO pins' default high impedance state
- The timers have different numbers of capture/compare registers, which affects the values of TAxIV: 0x0A is timer overflow for G2553, and 0x0E is timer overflow for FR6989
- The MSP-EXT430G2ET board (G2553 processor) includes a pullup resistor on the button. The MSP-EXP430FR6989 board does not. Therefore, the internal pullup resistor for the button gpio pin is enable in the FR6989 program.
