;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Assembly language file for Lab 1 in 55:036 (Embedded Systems)
; Spring 2014, The University of Iowa.
;
; LEDs are connected via a 470 Ohm resistor from PB1, PB2 to Vcc
;
; Ben Weinberg & Daniel Nunez
;
.include "tn45def.inc"
.cseg
.org 0

; Configure PB1 and PB2 as output pins.
      sbi   DDRB,1      ; PB1 is now output
      sbi   DDRB,2      ; PB2 is now output

; Main loop follows.  Toggle PB1 and PB2 out of phase.
; Assuming there are LEDs and current-limiting resistors
; on these pins, they will blink out of phase.
   loop:
      sbi   PORTB,1     ; LED at PB1 off
      cbi   PORTB,2     ; LED at PB2 on 
      rcall delay_long  ; Wait
      cbi   PORTB,1     ; LED at PB1 on
      sbi   PORTB,2     ; LED at PB2 off  
      rcall delay_long  ; Wait
      rjmp   loop

; Generate a delay using three nested loops that does nothing. 
; With a 10 MHz clock, the values below produce ~261 ms delay.
   delay_long:			;														Goal: 0.2484 s || .0000001s/op = 2,484,000ops
      ldi   r23,18       ; r23 <-- Counter for outer loop						1 op
  d1: ldi   r24,193     ; r24 <-- Counter for level 2 loop						1 op			d1 = x3(4 + d2) - 1
  d2: ldi   r25,178     ; r25 <-- Counter for inner loop						1 op			d2 = x2(4 + d3) - 1  
  d3: dec   r25			; decrement r25 by 1									1 op			d3 = 4x1 - 1
      nop               ; no operation											1 op			
      brne  d3			; branch if not equal (if z=0 then go back else cont)	2/1 op	
      dec   r24			; decrement r24 by 1									1 op
      brne  d2			; branch if not equal (if z=0 then go back else cont)	2/1 op	
      dec   r23			; decrement r23 by 1									1 op
      brne  d1			; branch if not equal (if z=0 then go back else cont)	2/1 op	
      ret				; return back											4 op
.exit

