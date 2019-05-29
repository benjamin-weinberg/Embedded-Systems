#### Objective:
The objective of this lab is to assemble, test, and debug the ATtiny45 development board (Kit A) that you will use during the first few labs.

#### Description:
With a 10 MHz external clock, the supplied routine delay_long provides a delay of about 0.261 s. Modify this routine (nothing else) in the supplied assembly language program so that each LED blinks exactly (or as close as possible) 0.2484 s long (i.e., 0.2484 s ON & 0.2484 s OFF). To accomplish this you must analyze delay_long, count the number of clock cycles and adjust the loop counter and/or insert ‘nop’ (no operation) instructions in delay_long. When you make these adjustments, account for the overhead of calling delay_long, and the other instructions in the main loop. The aim is to have the LEDs on/off for exactly 0.2484 s long.
