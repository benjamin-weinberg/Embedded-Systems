#### Objective:
Gain some experience with digital I/O, timers, complex timing issues, and rotary pulse generators (RPGs).

#### Description:
For this lab, you will construct a simple system to control the duty cycle of a square wave. This device will be capable of varying the duty cycle of a 3.96 kHz square wave in a range from 30% to 70%. One should be able to adjust the duty cycle in increments of <= 1%. The duty cycle adjustment is done using a rotary pulse generator. Turning the pulse generator clockwise should increase the duty cycle. When the maximum duty cycle is attained, further clockwise rotation of the pulse generator should have no effect. Turning the pulse generator counter-clockwise should decrease the duty cycle of the square wave until the minimum duty cycle is attained. At this point, further counter-clockwise rotation should have no effect. To demonstrate your program, you should use an oscilloscope and observe the waveform behavior as the pulse generator is turned.

To receive full credit, your design must satisfy the following requirements:
* The frequency should be within +/- 1% of the nominal 3.96 kHz over the entire range of the duty cycle.
Must use timer hardware.
* It should be capable of generating duty cycles over the entire range from 30% through 70% in no more than 1% increments.
While monitoring the generated waveform on an oscilloscope, it should be easily possible to adjust the device to any duty cycle within the specified range of operation (to within the resolution error of the AVR timers).
* Do not use interrupts.
* Do not use the microcontroller's on-board PWM module.
* The lab must be implemented in assembly language.
* Utilize subroutines to organize your program.
* Use the external 10 MHz quartz crystal to achieve more accurate timing (install both clock jumpers (JP1) and program the oscillator fuses accordingly)!

Test your implementation. For example, once your device is operating, adjust it to produce a waveform with the minimum possible duty cycle. Confirm that you meet the duty cycle and frequency specification. Now adjust the duty cycle to the maximum value and repeat the process.

Note that there will be extra credit for the rate sensitive adjustment of the duty cycle (optional).
