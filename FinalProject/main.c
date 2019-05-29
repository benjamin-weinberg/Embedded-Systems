#define F_CPU 8000000			//telling controller crystal frequency attached

#include <avr/io.h>				//header to enable data flow control over pins
#include <stdlib.h>
#include <util/delay.h>			//header to enable delay function in program
#include <avr/interrupt.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <avr/pgmspace.h>     //  Routine for FLASH (program memory)

#define BAUD_RATE 9600        // Baud rate. The usart_int routine

// Variables and #define for the RX ring buffer.
#define RX_BUFFER_SIZE 64
unsigned char rx_buffer[RX_BUFFER_SIZE];
volatile unsigned char rx_buffer_head;
volatile unsigned char rx_buffer_tail;

// Function prototypes.
unsigned char uart_buffer_empty(void);
void usart_prints(const char *ptr);
void usart_printf(const char *ptr);
void usart_init(void);
void usart_putc(const char c);
unsigned char usart_getc(void);
void getDistAndPrint(const int angle);

#define Trigger_pin		PIND7	// trigger pin for us sensor
#define SERVO_PIN		PIND4	// servo "PWM" output
#define SERVO_SET_LOOPS 20		// number of times to send "PWM" pulse

int TimerOverflow = 0;

ISR(TIMER1_OVF_vect)
{
	TimerOverflow++;			// increment timer overflow count for long pulses
}

int main(void)
{
	
	usart_init();									// initialize serial to computer
	
	
	DDRD |= (1<<Trigger_pin) | (1<<SERVO_PIN);		// setup the servo pin and trigger pin as an output
	PORTD = 0xFF;									// turn on pull-up
	
	sei();											// enable global interrupt
	TIMSK1 = (1 << TOIE1);							// timer1 overflow interrupts
	TCCR1A = 0;										// set all bits to 0, normal operation

	while(1)
	{
		
		// set servo to 0 degrees
		for(int i = 0; i < SERVO_SET_LOOPS; i++){
			PORTD |= (1<<SERVO_PIN);
			_delay_us(570);
			PORTD &=~(1<<SERVO_PIN);
			_delay_us(20000);
		}
		
		getDistAndPrint(0);
		
		// set servo to 30 degrees
		for(int i = 0; i < SERVO_SET_LOOPS; i++){
			PORTD |= (1<<SERVO_PIN);
			_delay_us(833);
			PORTD &=~(1<<SERVO_PIN);
			_delay_us(20000);
		}
		
		getDistAndPrint(30);
		
		// set servo to 60 degrees
		for(int i = 0; i < SERVO_SET_LOOPS; i++){
			PORTD |= (1<<SERVO_PIN);
			_delay_us(1166);
			PORTD &=~(1<<SERVO_PIN);
			_delay_us(20000);
		}
		
		getDistAndPrint(60);
		
		// set servo to 90 degrees
		for(int i = 0; i < SERVO_SET_LOOPS; i++){
			PORTD |= (1<<SERVO_PIN);
			_delay_us(1500);
			PORTD &=~(1<<SERVO_PIN);
			_delay_us(20000);
		}
		
		getDistAndPrint(90);
		
		// set servo to 120 degrees
		for(int i = 0; i < SERVO_SET_LOOPS; i++){
			PORTD |= (1<<SERVO_PIN);
			_delay_us(1833);
			PORTD &=~(1<<SERVO_PIN);
			_delay_us(20000);
		}
		
		getDistAndPrint(120);
		
		// set servo to 150 degrees
		for(int i = 0; i < SERVO_SET_LOOPS; i++){
			PORTD |= (1<<SERVO_PIN);
			_delay_us(2166);
			PORTD &=~(1<<SERVO_PIN);
			_delay_us(20000);
		}
		
		getDistAndPrint(150);
		
		//set servo to 180 degrees
		for(int i = 0; i < SERVO_SET_LOOPS; i++){
			PORTD |= (1<<SERVO_PIN);
			_delay_us(2500);
			PORTD &=~(1<<SERVO_PIN);
			_delay_us(19000);
		}
		
		getDistAndPrint(180);
		
	}
}


void getDistAndPrint(const int angle){
	_delay_ms(50); // insure the servo is set
	
	char string[50];
	long count;
	double distance;
	
	// send 10us trigger to the ultrasonic sensor
	PORTD |= (1 << Trigger_pin);
	_delay_us(10);
	PORTD &= (~(1 << Trigger_pin));
			
	// setup timer
	TCNT1 = 0;
	TCCR1B = 0x41;
	TIFR1 |= (1<<ICF1);
	TIFR1 |= (1<<TOV1);

	// Calculate width of Echo by Input Capture (ICP)
	while ((TIFR1 & (1 << ICF1)) == 0);					// Wait for rising edge (echo to go high)
	TCNT1 = 0;											// Clear timer
	TCCR1B = 0x01;										// Capture on falling edge, No prescaler
	TIFR1 |= (1<<ICF1);									// Clear ICP flag
	TIFR1 |= (1<<TOV1);									// Clear Timer Overflow flag
	TimerOverflow = 0;									// Clear Timer overflow count

	while ((TIFR1 & (1 << ICF1)) == 0);					// Wait for echo to go low
	count = ICR1 + (65535 * TimerOverflow);				// Take total number of counts
	distance = (double)count / 466.47;					// With 8MHz freq and speed of sound as 343m/s, calculate distance

	// print the distance to serial
	sprintf(string, "Angle: %i Degrees \r\nDistance: %lf cm \r\n\r\n", angle, distance);
	usart_prints(string);
}


void usart_init(void)
{
	// Configures the USART for serial 8N1 with
	// the Baud rate controlled by a #define.

	unsigned short s;
	
	// Set Baud rate, controlled with #define above.
	
	s = (double)F_CPU / (BAUD_RATE*16.0) - 1.0;
	UBRR0H = (s & 0xFF00);
	UBRR0L = (s & 0x00FF);

	// Receive complete interrupt enable: RXCIE0
	// Receiver & Transmitter enable: RXEN0,TXEN0

	UCSR0B = (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);

	// Along with UCSZ02 bit in UCSR0B, set 8 bits
	
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00)|(1<<UPM01)|(1<<USBS0);
	
	DDRD |= (1<< 1);         // PD0 is output (TX)
	DDRD &= ~(1<< 0);        // PD1 is input (Rx)
	
	// Empty buffers
	
	rx_buffer_head = 0;
	rx_buffer_tail = 0;
}


void usart_printf(const char *ptr){

	// Send NULL-terminated data from FLASH.
	// Uses polling (and it blocks).

	char c;

	while(pgm_read_byte_near(ptr)) {
		c = pgm_read_byte_near(ptr++);
		usart_putc(c);
	}
}

void usart_putc(const char c){
	// Send "c" via the USART.  Uses poling
	// (and it blocks). Wait for UDRE0 to become
	// set (=1), which indicates the UDR0 is empty
	// and can accept the next character.

	while (!(UCSR0A & (1<<UDRE0)));
	UDR0 = c;
}

void usart_prints(const char *ptr){
	// Send NULL-terminated data from SRAM.
	// Uses polling (and it blocks).

	while(*ptr) {
		while (!( UCSR0A & (1<<UDRE0)));
		UDR0 = *(ptr++);
	}
}

unsigned char usart_getc(void)
{
	// Get char from the receiver buffer.  This
	// function blocks until a character arrives.
	
	unsigned char c;
	
	// Wait for a character in the buffer.

	while (rx_buffer_tail == rx_buffer_head);
	
	c = rx_buffer[rx_buffer_tail];
	if (rx_buffer_tail == RX_BUFFER_SIZE-1)
	rx_buffer_tail = 0;
	else
	rx_buffer_tail++;
	return c;
}

unsigned char uart_buffer_empty(void)
{
	// Returns TRUE if receive buffer is empty.
	
	return (rx_buffer_tail == rx_buffer_head);
}
