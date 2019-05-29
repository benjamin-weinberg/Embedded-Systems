#### Objective: 
Gain some experience in writing an assembly language program as well as connecting external hardware to the microcontroller.

#### Description:
In this lab, students will build a hexadecimal up/down counter using the ATtiny45 microcontroller (Kit A) in combination with a power logic 8-bit shift register, a 7-segment LED display, and a pushbutton switch. When the power is turned on, the 7-segment display will show "0" and the counter will be in increment mode. The pushbutton switch is used for mode selection, counter increment/decrement, and counter reset as described below in more detail.

Use the following approach for displaying hexadecimal numbers on the 7-segment display.

Selecting between increment and decrement mode - The user can toggle between increment and decrement mode by pressing and holding the pushbutton for more than one, but less than two seconds. To differentiate between both modes, the decimal point (DP) of the 7-segment LED display will be used. If the counter is in increment mode, the DP LED will be off. In contrast, if the counter is in decrement mode, the DP LED will be on.

Counter increment - When the user presses the pushbutton switch for less than one second in increment mode, the counter contents will be incremented by one and the display will be updated accordingly. If the counter displays “F” and a counter increment event occurs, the counter will overflow and display “0”.

Counter decrement - When the user presses the pushbutton switch for less than one second in decrement mode, the counter contents will be decremented by one and the display will be updated accordingly. If the counter displays “0” and a counter decrement event occurs, the counter will overflow and display “F”.

Counter reset - To reset the counter to “0” (increment mode), the user needs to press and hold the pushbutton switch for two or more seconds.

Note that all counter, mode selection, and reset action must be applied once the pushbutton is released.

Design the interface to the 7-segment display such that the current to drive a segment is 11 mA. Details regarding the power logic shift register IC, the 7-segment displays, connection to ATtiny45, etc. will be covered in class. Each group will get one power shift register IC and one 7-segment display module from one of the TAs.
