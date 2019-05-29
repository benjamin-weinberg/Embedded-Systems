;
; EmbeddedLab2.asm
;
; Created: 2/8/2019 10:27:17 AM
; Author : wnbrg
;

;R19 hold 0 when counting up and 1 when counting down
;R18 holds current counter
;R17 holds loop counter for SR
;R16 holds the pattern to display

; put code here to configure I/O lines
; connected to TPIC6C595 as output
	sbi DDRB,0; SRCK
	sbi DDRB,1; SERIN
	sbi DDRB,2; RCK
	cbi DDRB,3; Push Button
; start main program

ldi R18, 0
rcall resetTo0
mainloop:
	sbis PINB,3
		rcall push_down
	rjmp mainloop

push_down:
	.Def timer = r20
	ldi timer, 0
	timerLoop: 
		rcall delay
		inc timer
		sbic PINB, 3
		rjmp timer_break


		rjmp timerLoop

timer_break:
	cpi timer, 10
	brlo incOrDec
	cpi timer, 20
	brlo switch_mode
	brsh resetTo0
	ret

switch_mode:
	tst R19
	breq if_start4
		ldi R19, 0
	rjmp end_if4
if_start4:
	ldi R19, 1
end_if4:
	rcall findPattern
	ret

incOrDec:
	tst R19
	breq if_start5
		rcall count_down
	rjmp end_if5
if_start5:
		rcall count_up
end_if5:
	ret
	

count_up:
	cpi R18,0x0F
	breq if_start2
	inc R18
	rjmp if_end2
if_start2:
	rcall resetTo0
if_end2:
	rcall findPattern
	ret

count_down:
	cpi R18,0x00
	breq if_start3
	dec R18
	rjmp if_end3
if_start3:
	rcall resetToF
if_end3:
	rcall findPattern
	ret
	
resetTo0:
	ldi r19, 0 
	ldi r18, 0
	rcall findPattern
	ret

resetToF:
	ldi r18, 0x0F
	rcall findPattern
	ret

display:
; see if you should add one to display D.P. (currently decrementing)
	tst R19
	brne if_start1
	rjmp end_if1
if_start1:
	inc R16
end_if1:

	; backup used registers on stack
	push R16
	push R17
	in R17, SREG
	push R17

	ldi R17, 8 ; loop --> test all 8 bits

loop:
	rol R16 ; rotate left trough Carry
	BRCS set_ser_in_1 ; branch if Carry set
	; put code here to set SER_IN to 0
		cbi PORTB,1
	rjmp end
set_ser_in_1:
	; put code here to set SER_IN to 1
		sbi PORTB,1
end:
	; put code here to generate SRCK pulse
		sbi PORTB,0
		nop
		nop
		nop
		cbi PORTB,0
	dec R17
	brne loop
	; put code here to generate RCK pulse
		sbi PORTB,2
		nop
		nop
		nop
		cbi PORTB,2
	; restore registers from stack
	pop R17
	out SREG, R17
	pop R17
	pop R16
	ret 

disp0:
	ldi R16, 0xF6 ; load pattern to display
	rcall display ; call display subroutine
	ret
disp1:
	ldi R16, 0x06 ; load pattern to display
	rcall display ; call display subroutine
	ret
disp2:
	ldi R16, 0xEC ; load pattern to display
	rcall display ; call display subroutine
	ret
disp3:
	ldi R16, 0xCE ; load pattern to display
	rcall display ; call display subroutine
	ret
disp4:
	ldi R16, 0x1E ; load pattern to display
	rcall display ; call display subroutine
	ret
disp5:
	ldi R16, 0xDA ; load pattern to display
	rcall display ; call display subroutine
	ret
disp6:
	ldi R16, 0xFA ; load pattern to display
	rcall display ; call display subroutine
	ret
disp7:
	ldi R16, 0x86 ; load pattern to display
	rcall display ; call display subroutine
	ret
disp8:
	ldi R16, 0xFE ; load pattern to display
	rcall display ; call display subroutine
	ret
disp9:
	ldi R16, 0xDE ; load pattern to display
	rcall display ; call display subroutine
	ret
dispA:
	ldi R16, 0xBE ; load pattern to display
	rcall display ; call display subroutine
	ret
dispB:
	ldi R16, 0x7A ; load pattern to display
	rcall display ; call display subroutine
	ret
dispC:
	ldi R16, 0xF0 ; load pattern to display
	rcall display ; call display subroutine
	ret
dispD:
	ldi R16, 0x6E ; load pattern to display
	rcall display ; call display subroutine
	ret
dispE:
	ldi R16, 0xF8 ; load pattern to display
	rcall display ; call display subroutine
	ret
dispF:
	ldi R16, 0xB8 ; load pattern to display
	rcall display ; call display subroutine
	ret

findPattern: ;R15 holds the number we need to display, find the pattern
	cpi R18,0
	breq disp0
	cpi R18,1
	breq disp1
	cpi R18,2
	breq disp2
	cpi R18,3
	breq disp3
	cpi R18,4
	breq disp4
	cpi R18,5
	breq disp5
	cpi R18,6
	breq disp6
	cpi R18,7
	breq disp7
	cpi R18,8
	breq disp8
	cpi R18,9
	breq disp9
	cpi R18,10
	breq dispA
	cpi R18,11
	breq dispB
	cpi R18,12
	breq dispC
	cpi R18,13
	breq dispD
	cpi R18,14
	breq dispE
	cpi R18,15
	breq dispF

delay: ;Delays for 1/10 of a sec
      ldi   r23,4      ; r23 <-- Counter for outer loop
  d1: ldi   r24,93     ; r24 <-- Counter for level 2 loop 
  d2: ldi   r25,53     ; r25 <-- Counter for inner loop
  d3: dec   r25
      nop               ; no operation 
      brne  d3 
      dec   r24
      brne  d2
      dec   r23
      brne  d1
      ret