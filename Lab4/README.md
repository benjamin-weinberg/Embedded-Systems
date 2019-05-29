#### Objective: 
Gain experience with advanced timer/counter functionality, pulse width modulation (PWM), internal/external interrupts, and LCDs.

#### Description:
For this lab, you will implement hard and software (assembly language) to operate a cooling fan (see a TA for check out) at different speeds by utilizing a PWM approach. The duty cycle of the PWM must be adjustable by turning the RPG (CW rotation: increase; CCW rotation: decrease). In addition, you will implement functionality to monitor the operation of the cooling fan. For this lab, you will need to purchase, assemble, and use the 28-pin AVR development board with an 8 MHz crystal for frequency generation. The functionality to be implemented is given below.

* Build a suitable circuit connecting the fan, RPG, LCD, and push button to the microcontroller. Justify your design decisions.
* Use Timer/Couter0’s Waveform Generation functionality for generating a PWM signal on pin OC0B (PWM signal for fan). The PWM frequency must be set at 40 kHz (fixed), and the duty cycle must be adjustable between 1% and 100% by turning the RPG (increments <1%). Alternatively, the 8-bit Timer/Couter2 can be used instead of Timer/Couter0.
* Implement the following fan speed monitoring functionality (modes A and B), which must be selectable by using a button, by evaluating the fan’s open collector tachometer output. Repeatedly pushing the button must allow to cycle through two monitoring modes: mode A, mode B, mode A, …
* Mode A: This mode must enable detecting when the fan stops running and doesn’t provide airflow for cooling.
* Mode B: Mode B must enable detecting fan speeds below (2520 rpm --> 42 Hz).

At all times, the following information must be displayed on the LCD display: duty cycle in %, selected monitoring mode (A or B), as well as monitoring status. Follow the formatting examples given below.

```
DC = 63.5%
Mode A: OK
```

```
DC = 42.0%
Mode A: ALARM
```

```
DC = 78.5%
Mode B: OK
```

```
DC =  3.0%
Mode B: LOW RPM
```

Use timers, interrupts, and other hardware resources of the ATmega88PA for implementing above described functionality.
