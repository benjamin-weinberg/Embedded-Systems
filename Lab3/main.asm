;
; AssemblerApplication1.asm
;
; Created: 2/22/2019 12:58:25 PM
; Author : Daniel Nunez & Ben Weinberg
;

; Replace with your application code

; connected to TPIC6C595 as output
	cbi DDRB,0; B connection
	cbi DDRB,1; A connectoin
	sbi DDRB,2; LED
; start main program

.def oldA = r16		; holding old value of rpg
.def oldb = r17		; holding old value of rpg
.def newA = r18		; holding new value of rpg
.def newB = r19		; holding new value of rpg
.def eorB = r20		; result of eor
.def eorA = r21		; result of eor
.def tmp1 = r23		; Use r23 for temporary variables
.def tmp2 = r24		; Use r24 for temporary values 
.def count = r25	; preloaded value
.def variable = r26	; value that stores how much to change duty cycle by

.def rpg = r27


ldi count, 150
ldi tmp1, 0x02
out TCCR0B,tmp1 ; Restart timer
ldi variable, 125

start:
	in rpg,PINB			; read them in at the same time

	mov oldA, newA		; copy new A to old A
	mov oldB, newB		; copy old b to new b

	sbrs rpg, 0			; check bits to get a&b
		ldi newB,1
	sbrc rpg, 0
		ldi newB,0
	sbrs rpg,1
		ldi newA,1
	sbrc rpg,1
		ldi newA,0

	mov eorA, newA
	mov eorB, newB
	eor eorA, oldB		; eor bits to check for rotation
	eor eorB, oldA		; eor bits to check for rotation

	cp eorA,eorB 
		breq startWave	; start the wave, no rotation
	cp eorA,eorB	
		brne change		; change the variable, there is a rotation

change:
	cpi eorb,00000001	; CW turn, skip to increment duty cycle
		breq incDC
	cpi eorb,00000010	; CCW turn, skip to decrement duty cycle
		brne decDC

decDC:			; CCW turn
	cpi variable, 0
		brlt end_if		; check for bounds
	dec variable
	end_if:
	rjmp startWave

incDC:			; CW turn
	cpi variable, 126
		brge end_if2	; check for bounds
	inc variable
	end_if2:
	rjmp startWave
	
startWave:
	;37+var	// how long high
	mov tmp1, variable
	subi tmp1, -37
	mov count, tmp1
	sbi PORTB,2
	rcall delay
	rcall wait
	cbi PORTB,2
	;166-var // how long low
	ldi tmp1, 167
	mov count, variable
	sub tmp1, count
	mov count, tmp1
	rcall delay	
	rcall wait
	rjmp start

; Wait for TIMER0 to roll over.
delay:
	; Stop timer 0.
	in tmp1,TCCR0B ; Save configuration
	ldi tmp2,0x00 ; Stop timer 0
	out TCCR0B,tmp2
	; Clear over flow flag.
	in tmp2,TIFR ; tmp <-- TIFR
	sbr tmp2,1<<TOV0 ; Clear TOV0, write logic 1
	out TIFR,tmp2
	; Start timer with new initial count
	out TCNT0,count ; Load counter
	out TCCR0B,tmp1 ; Restart timer
	ret

wait:
	in tmp2,TIFR ; tmp <-- TIFR
	sbrs tmp2,TOV0 ; Check overflow flag
	rjmp wait
	ret