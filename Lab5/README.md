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
