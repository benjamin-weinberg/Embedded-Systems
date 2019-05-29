#### Objective: 
Gain experience with C-based programming of AVR microcontrollers, serial interface protocols (I2C, RS232,...), nonvolatile memory (EEPROM), ADC, and DAC.

#### Description:
You will build a remote controllable analog data logging system (C language) using the built-in A/D converter (ADC) of the ATmega88PA controller (Vref = 5 V) and the MAX518, an external two-channel D/A converter (DAC) with an I2C interface. Important aspects of the utilized ADC and DAC will be covered in class (review the relevant lecture notes). The MAX518 chip and associated components are available at the CoE Electronic shop.

The analog interface system will have an RS232 interface (9600 8E2) that will be connected to a PC. The PC user will be able to trigger a single voltage measurement, storage of analog voltage measurements in EEPROM, data retrieval from EEPROM, as well as reproduction of stored measurements on one of the DAC channels by means of commands sent through the RS232 interface. 

The system will implement the following commands.

Command	| Function | Arguments
--------|----------|----------
M	| get single voltage measurement from ADC |	no arguments
S:a,n,t |	store ADC measurements in EEPROM	| a … start address (integer, 0≤ a ≤ 510) n … number of measurements (integer, 1 ≤ n ≤ 20) t ... time between measurements (integer, 1 ≤ dt ≤ 10 s)
R:a,n |	retrieve and display measurements from EEPROM	| a … start address (integer, 0≤ a ≤ 510) n … number of measurements (integer, 1 ≤ n ≤ 20) 
E:a,n,t,d	| retrieve measurements from EEPROM and write results to DAC | a … start address (integer, 0≤ a ≤ 510) n … number of measurements (integer, 1 ≤ n ≤ 20) t ... time between measurements (integer, 1 ≤ dt ≤ 10 s) d … DAC channel number (integer, d∈ {0,1}) 

Note that stored 10-bit ADC values must be converted to the closest 8-bit integer value such that the DAC quantization error is minimal.

Note the ATmega88PA ADC must be used in 10-bit mode! 

Bonus points will be given for a) utilizing timers/counters with interrupts to generate the required delays between ADC read and DAC write operations and b) implementation of appropriate checks and error notifications.

Here is the nominal message exchange between the person typing on the terminal keyboard and analog interface system, which will send information back to the terminal screen. Text shown in BOLD is typed by the user on the keyboard. Non-bold text represents responses by the analog interface system:
```
M
v=1.689 V

S:0,5,1
t=0 s, v=4.448 V, addr: 0
t=1 s, v=2.012 V, addr: 2
t=2 s, v=1.558 V, addr: 4
t=3 s, v=0.205 V, addr: 6
t=4 s, v=0.488 V, addr: 8

R:0,2 
v=4.448 V
v=2.012 V

E:0,3,1,0
t=0 s, DAC channel 0 set to 4.453 V (228d)
t=1 s, DAC channel 0 set to 2.012 V (103d)
t=2 s, DAC channel 0 set to 1.563 V (80d)
```
