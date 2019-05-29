#define F_CPU 8000000L        // This should match the processor speed

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <ctype.h>
#include "i2cmaster.h"
#include <avr/interrupt.h>
#include <avr/pgmspace.h>     //  Routine for FLASH (program memory)
#include <string.h>

#define BAUD_RATE 9600        // Baud rate. The usart_int routine
#define REF_AVCC (1<<REFS0)  // reference = AVCC = 5 V
#define REF_INT  (1<<REFS0)|(1<<REFS1) // internal reference 2.56 V

#define DevDAC 0b01011110 // I2C address of DAC
#define I2C_WRITE 0
#define I2C_READ 1

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
uint16_t adc_read(void);

void setValue(unsigned char channel, float voltage);
void resetDAC(unsigned char channel);
void EEPROM_write(unsigned int uiAddress, uint8_t ucData);
uint8_t EEPROM_read(unsigned int uiAddress);


int main(void)
{
    unsigned char  c;
	
	char str[50];
    int totala, totaln, totalt, totald;
    int d1, d2, d3,comma;
    
    sei();                  // Enable interrupts
	
	usart_init();           // Initialize the USART
// 	usart_printf(fdata);    // Print a string from FLASH
// 	usart_prints(sdata);    // Print a string from SRAM

    // initialize ADC
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	ADMUX |= (1<<REFS0);                                                        // set reference and channel


    usart_prints("\r\n\r\nWelcome!\r\n\r\n");
    
    i2c_init();                                                                 // init i2c
    resetDAC(0);                                                                // reset channel 0
    resetDAC(1);                                                                // reset channel 1
    
	setValue(0,2.0);
	setValue(1,4.0);
	
	while(1){
        c = usart_getc();
        usart_putc(c);                                                          // Echo back the character
        switch(c){
            case 'M':
            /*
                Get measurement from ADC
                
                FORMAT: 'M'
                no args
            */  
                usart_prints("\r\n");                                           // go to next line
                uint16_t adcval = adc_read();                                      // read and print the value of the ADC
                break;                                                          // end case
                
            case 'S': // 
            /*
                Store ADC measurement in EEPROM  
                
                FORMAT: 'S:a,n,t'
                a ... start address (integer, 0? a ? 510) 
                n ... number of measurements (integer, 1 ? n ? 20) 
                t ... time between measurements (integer, 1 ? dt ? 10 s)
            */
                // Check for the ':'
                c = usart_getc();                                               // get the character
                usart_putc(c);                                                  // echo back the character
                if(c != ':'){                                                   // error check for ':'
					sprintf(str, "\r\nExpected ':' but got '%c'\r\n", c);
                    usart_prints(str);
                    break;                                                      // break out of case (get new input)
                }
                
                comma = 0;
                // next value needs to be a digit
                c = usart_getc();                                               // get the character
                usart_putc(c);                                                  // echo back the character
                if (!isdigit((int)c)){                                          // error check for a digit
		            sprintf(str, "\r\nExpected a number but got '%c'\r\n", c);
		            usart_prints(str);
		            break;
				}
                d1 = (int) c - '0';
                totala = d1;
                
                // next value needs to be a digit or a comma
                c = usart_getc();                                               // get the character
                usart_putc(c);                                                  // echo back the character
                if ((!isdigit((int)c)) && (c!=',')){                            // error check for a digit
		            sprintf(str, "\r\nExpected a number or ',' but got '%c'\r\n", c);
		            usart_prints(str);
		            break;
				}
                if (isdigit((int) c)){
                    d2 = (int) c - '0';
                    totala = d1*10 + d2;
                } 
                else comma = 1;
               
                // if previous was not a comma, next needs to be an int or comma
                if (comma == 0){
                    c = usart_getc();                                               // get the character
                    usart_putc(c);                                                  // echo back the character
                    if ((!isdigit((int)c)) && (c!=',')){                            // error check for a digit
                        sprintf(str, "\r\nExpected a number or ',' but got '%c'\r\n", c);
                        usart_prints(str);
                        break;
                    }
                    if (isdigit((int) c)){
	                    d3 = (int) c - '0';
	                    totala = (d1*100) + (d2*10) + d3;
                    }
                    else comma = 1;
                    if (totala > 510){
                        usart_prints("\r\nNumber is too large!");
                        break; 
                    }
                }
                
                // if previous was not a comma, next needs to be a comma
                if (comma == 0){
                    c = usart_getc();                                               // get the character
                    usart_putc(c);                                                  // echo back the character
                    if ((!isdigit((int)c)) && (c!=',')){                            // error check for a digit
                        sprintf(str, "\r\nExpected a ',' but got '%c'\r\n", c);
                        usart_prints(str);
                        break;
                    }
                }    

                comma = 0;
                // next value needs to be a digit
                c = usart_getc();                                               // get the character
                usart_putc(c);                                                  // echo back the character
                if (!isdigit((int)c)){                                          // error check for a digit
		            sprintf(str, "\r\nExpected a number but got '%c'\r\n", c);
		            usart_prints(str);
		            break;
				}
                d1 = (int) c - '0';
                totaln = d1;            
                
                // next value needs to be a digit or a comma
                c = usart_getc();                                               // get the character
                usart_putc(c);                                                  // echo back the character
                if ((!isdigit((int)c)) && (c!=',')){                            // error check for a digit
		            sprintf(str, "\r\nExpected a number or ',' but got '%c'\r\n", c);
		            usart_prints(str);
		            break;
				}
                if (isdigit((int) c)){
                    d2 = (int) c - '0';
                    totaln = d1*10 + d2;
                    if (totaln > 20){
                        usart_prints("\r\nNumber is too large!");
                        break; 
                    }
                } 
                else comma = 1;                
                
                // if previous was not a comma, next needs to be a comma
                if (comma==0){
                    c = usart_getc();                                               // get the character
                    usart_putc(c);                                                  // echo back the character
                    if ((!isdigit((int)c)) && (c!=',')){                            // error check for a digit
                        sprintf(str, "\r\nExpected a ',' but got '%c'\r\n", c);
                        usart_prints(str);
                        break;
                    }
                }       

                comma = 0;
                // next value needs to be a digit
                c = usart_getc();                                               // get the character
                usart_putc(c);                                                  // echo back the character
                if (!isdigit((int)c)){                                          // error check for a digit
		            sprintf(str, "\r\nExpected a number but got '%c'\r\n", c);
		            usart_prints(str);
		            break;
				}
                d1 = (int) c - '0';
                totalt = d1;            
                
                // next value needs to be a digit or a comma
                c = usart_getc();                                               // get the character
                usart_putc(c);                                                  // echo back the character
                if ((!isdigit((int)c)) && (c!=',')){                            // error check for a digit
		            sprintf(str, "\r\nExpected a number or ',' but got '%c'\r\n", c);
		            usart_prints(str);
		            break;
				}
                if (isdigit((int) c)){
                    d2 = (int) c - '0';
                    totalt = d1*10 + d2;
                    if (totalt > 10){
                        usart_prints("\r\nNumber is too large!");
                        break; 
                    }
                } 
				
				// implementation 
				for(int j = 0;j<totaln;j++){
					sprintf(str, "\r\nt = %d s, ", (j*totalt));
					usart_prints(str);
					
					uint16_t adcval = adc_read();
					uint8_t partA = (uint8_t)((adcval & 0xFF00) >> 8);
					uint8_t partB = (uint8_t)(adcval & 0x00FF);
					unsigned int address = totala+(j*2);
					EEPROM_write(address, partA);
					EEPROM_write(address+1, partB);
					sprintf(str, " addr: %d", address);
					usart_prints(str);
					for(int i = 0; i<totalt;i++){
						_delay_ms(1000);
					}
				}
				
				
				
				
                break; // end case
            
            case 'R': 
            /*
                Retrieve and display measurements from EEPROM 
                
                FORMAT: 'R:a,n'
                a ... start address (integer, 0? a ? 510) 
                n ... number of measurements (integer, 1 ? n ? 20) 
            */
                // Check for the ':'
                c = usart_getc();    // get the character
                usart_putc(c);      // echo back the character
                if(c != ':'){
                    printf("\r\n Expected ':' but got '%c'\r\n", c);
                    break; // break out of case (get new input)
                }
                
                comma = 0;
                // next value needs to be a digit
                c = usart_getc();                                               // get the character
                usart_putc(c);                                                  // echo back the character
                if (!isdigit((int)c)){                                          // error check for a digit
	                sprintf(str, "\r\nExpected a number but got '%c'\r\n", c);
	                usart_prints(str);
	                break;
                }
                d1 = (int) c - '0';
                totala = d1;
                
                // next value needs to be a digit or a comma
                c = usart_getc();                                               // get the character
                usart_putc(c);                                                  // echo back the character
                if ((!isdigit((int)c)) && (c!=',')){                            // error check for a digit
	                sprintf(str, "\r\nExpected a number or ',' but got '%c'\r\n", c);
	                usart_prints(str);
	                break;
                }
                if (isdigit((int) c)){
	                d2 = (int) c - '0';
	                totala = d1*10 + d2;
                }
                else comma = 1;
                
                // if previous was not a comma, next needs to be an int or comma
                if (comma == 0){
	                c = usart_getc();                                               // get the character
	                usart_putc(c);                                                  // echo back the character
	                if ((!isdigit((int)c)) && (c!=',')){                            // error check for a digit
		                sprintf(str, "\r\nExpected a number or ',' but got '%c'\r\n", c);
		                usart_prints(str);
		                break;
	                }
	                if (isdigit((int) c)){
		                d3 = (int) c - '0';
		                totala = (d1*100) + (d2*10) + d3;
	                }
	                else comma = 1;
	                if (totala > 510){
		                usart_prints("\r\nNumber is too large!");
		                break;
	                }
                }
                
                // if previous was not a comma, next needs to be a comma
                if (comma == 0){
	                c = usart_getc();                                               // get the character
	                usart_putc(c);                                                  // echo back the character
	                if ((!isdigit((int)c)) && (c!=',')){                            // error check for a digit
		                sprintf(str, "\r\nExpected a ',' but got '%c'\r\n", c);
		                usart_prints(str);
		                break;
	                }
                }

                comma = 0;
                // next value needs to be a digit
                c = usart_getc();                                               // get the character
                usart_putc(c);                                                  // echo back the character
                if (!isdigit((int)c)){                                          // error check for a digit
	                sprintf(str, "\r\nExpected a number but got '%c'\r\n", c);
	                usart_prints(str);
	                break;
                }
                d1 = (int) c - '0';
                totaln = d1;
                   
				
				
                // next value needs to be a digit or a comma
                c = usart_getc();                                               // get the character
                usart_putc(c);                                                  // echo back the character
                if ((!isdigit((int)c)) && (c!=',')){                            // error check for a digit
	                sprintf(str, "\r\nExpected a number or ',' but got '%c'\r\n", c);
	                usart_prints(str);
	                break;
                }
                if (isdigit((int) c)){
	                d2 = (int) c - '0';
	                totaln = d1*10 + d2;
	                if (totaln > 20){
		                usart_prints("\r\nNumber is too large!");
		                break;
	                }
                }
				
				
            /*
                Retrieve and display measurements from EEPROM 
                
                FORMAT: 'R:a,n'
                a ... start address (integer, 0? a ? 510) 
                n ... number of measurements (integer, 1 ? n ? 20) 
            */           
				
				// implementation
				for(int j = 0;j<totaln;j++){
					unsigned int address = totala+(j*2);
					sprintf(str, "\r\naddr: %d , ", address);
					usart_prints(str);
					
					uint8_t partA = EEPROM_read(address);
					uint8_t partB = EEPROM_read(address+1);
					uint16_t whole = ((uint16_t)partA << 8) | partB;
					
					float adcvalfloat = (float)whole*5/1023;
					sprintf(str, "v = '%.3f'", adcvalfloat);
					usart_prints(str);
				}
                
                break; // end case
                
            case 'E':
                /*
                    Retrieve measurements from EEPROM and write to DAC 
                    
                    FORMAT: 'E:a,n,t,d'
                    a ... start address (integer, 0? a ? 510) 
                    n ... number of measurements (integer, 1 ? n ? 20) 
                    t ... time between measurements (integer, 1 ? dt ? 10 s)
                    d ... DAC channel number (integer, d is {0 or 1}) 
                    
                    Note that stored 10-bit ADC values must be converted to the closest
                    8-bit integer value such that the DAC quantization error is minimal.
                */
                // Check for the ':'
                c = usart_getc();    // get the character
                usart_putc(c);      // echo back the character
                if(c != ':'){
                    printf("\r\n Expected ':' but got '%c'\r\n", c);
                    break; // break out of case (get new input)
                }
                
                
                comma = 0;
                // next value needs to be a digit
                c = usart_getc();                                               // get the character
                usart_putc(c);                                                  // echo back the character
                if (!isdigit((int)c)){                                          // error check for a digit
	                sprintf(str, "\r\nExpected a number but got '%c'\r\n", c);
	                usart_prints(str);
	                break;
                }
                d1 = (int) c - '0';
                totala = d1;
                
                // next value needs to be a digit or a comma
                c = usart_getc();                                               // get the character
                usart_putc(c);                                                  // echo back the character
                if ((!isdigit((int)c)) && (c!=',')){                            // error check for a digit
	                sprintf(str, "\r\nExpected a number or ',' but got '%c'\r\n", c);
	                usart_prints(str);
	                break;
                }
                if (isdigit((int) c)){
	                d2 = (int) c - '0';
	                totala = d1*10 + d2;
                }
                else comma = 1;
                
                // if previous was not a comma, next needs to be an int or comma
                if (comma == 0){
	                c = usart_getc();                                               // get the character
	                usart_putc(c);                                                  // echo back the character
	                if ((!isdigit((int)c)) && (c!=',')){                            // error check for a digit
		                sprintf(str, "\r\nExpected a number or ',' but got '%c'\r\n", c);
		                usart_prints(str);
		                break;
	                }
	                if (isdigit((int) c)){
		                d3 = (int) c - '0';
		                totala = (d1*100) + (d2*10) + d3;
	                }
	                else comma = 1;
	                if (totala > 510){
		                usart_prints("\r\nNumber is too large!");
		                break;
	                }
                }
                
                // if previous was not a comma, next needs to be a comma
                if (comma == 0){
	                c = usart_getc();                                               // get the character
	                usart_putc(c);                                                  // echo back the character
	                if ((!isdigit((int)c)) && (c!=',')){                            // error check for a digit
		                sprintf(str, "\r\nExpected a ',' but got '%c'\r\n", c);
		                usart_prints(str);
		                break;
	                }
                }

                comma = 0;
                // next value needs to be a digit
                c = usart_getc();                                               // get the character
                usart_putc(c);                                                  // echo back the character
                if (!isdigit((int)c)){                                          // error check for a digit
	                sprintf(str, "\r\nExpected a number but got '%c'\r\n", c);
	                usart_prints(str);
	                break;
                }
                d1 = (int) c - '0';
                totaln = d1;
                
                // next value needs to be a digit or a comma
                c = usart_getc();                                               // get the character
                usart_putc(c);                                                  // echo back the character
                if ((!isdigit((int)c)) && (c!=',')){                            // error check for a digit
	                sprintf(str, "\r\nExpected a number or ',' but got '%c'\r\n", c);
	                usart_prints(str);
	                break;
                }
                if (isdigit((int) c)){
	                d2 = (int) c - '0';
	                totaln = d1*10 + d2;
	                if (totaln > 20){
		                usart_prints("\r\nNumber is too large!");
		                break;
	                }
                }
                else comma = 1;
                
                // if previous was not a comma, next needs to be a comma
                if (comma==0){
	                c = usart_getc();                                               // get the character
	                usart_putc(c);                                                  // echo back the character
	                if ((!isdigit((int)c)) && (c!=',')){                            // error check for a digit
		                sprintf(str, "\r\nExpected a ',' but got '%c'\r\n", c);
		                usart_prints(str);
		                break;
	                }
                }

                comma = 0;
                // next value needs to be a digit
                c = usart_getc();                                               // get the character
                usart_putc(c);                                                  // echo back the character
                if (!isdigit((int)c)){                                          // error check for a digit
	                sprintf(str, "\r\nExpected a number but got '%c'\r\n", c);
	                usart_prints(str);
	                break;
                }
                d1 = (int) c - '0';
                totalt = d1;
                
                // next value needs to be a digit or a comma
                c = usart_getc();                                               // get the character
                usart_putc(c);                                                  // echo back the character
                if ((!isdigit((int)c)) && (c!=',')){                            // error check for a digit
	                sprintf(str, "\r\nExpected a number or ',' but got '%c'\r\n", c);
	                usart_prints(str);
	                break;
                }
                if (isdigit((int) c)){
	                d2 = (int) c - '0';
	                totalt = d1*10 + d2;
	                if (totalt > 10){
		                usart_prints("\r\nNumber is too large!");
		                break;
	                }
                }
                else comma = 1;
                
                // if previous was not a comma, next needs to be a comma
                if (comma==0){
	                c = usart_getc();                                               // get the character
	                usart_putc(c);                                                  // echo back the character
	                if ((!isdigit((int)c)) && (c!=',')){                            // error check for a digit
		                sprintf(str, "\r\nExpected a ',' but got '%c'\r\n", c);
		                usart_prints(str);
		                break;
	                }
                }
				
				// next value needs to be 1or 0
				c = usart_getc();                                               // get the character
				usart_putc(c);                                                  // echo back the character
				if (c!='0' && c!='1'){                            // error check for a digit
					sprintf(str, "\r\nExpected 1 or 0 but got '%c'\r\n", c);
					usart_prints(str);
					break;
				}
				totald = (int) c - '0';
				
				
                /*
                    Retrieve measurements from EEPROM and write to DAC 
                    
                    FORMAT: 'E:a,n,t,d'
                    a ... start address (integer, 0? a ? 510) 
                    n ... number of measurements (integer, 1 ? n ? 20) 
                    t ... time between measurements (integer, 1 ? dt ? 10 s)
                    d ... DAC channel number (integer, d is {0 or 1}) 
                    
                    Note that stored 10-bit ADC values must be converted to the closest
                    8-bit integer value such that the DAC quantization error is minimal.
                */
                
                
                // implementation
                for(int j = 0;j<totaln;j++){
	                unsigned int address = totala+(j*2);
	                sprintf(str, "\n\rt=%d s, DAC Chann: %d , ",(j*totalt), totald);
	                usart_prints(str);
	                
	                uint8_t partA = EEPROM_read(address);
	                uint8_t partB = EEPROM_read(address+1);
	                uint16_t whole = ((uint16_t)partA << 8) | partB;
	                
	                float adcvalfloat = (float)whole*5/1023;
	                sprintf(str, "V = '%.3f'", adcvalfloat);
	                usart_prints(str);
					
					setValue(totald, adcvalfloat);
					
	                for(int i = 0; i<totalt;i++){
		                _delay_ms(1000);
	                }
                }
				
				
                
                break; // end case
            
            default: 
                usart_prints("\r\n Expected 'M', 'S', 'R', or 'E'");
                break; // break out of case (get new input)
        }
        
        
        
        usart_prints("\r\n=====\r\n");
        
	}
	return(1);
}


ISR(USART_RX_vect)
{
	// UART receive interrupt handler.
	// To do: check and warn if buffer overflows.
	
	char c = UDR0;
	rx_buffer[rx_buffer_head] = c;
	if (rx_buffer_head == RX_BUFFER_SIZE - 1)
	rx_buffer_head = 0;
	else
	rx_buffer_head++;
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

uint16_t  adc_read(void)
{
	ADCSRA |= (1<<ADSC);                            // start conversion
	while(ADCSRA & (1<<ADSC));	                    // wait for conversion complete

	float adcvalfloat = (float)ADC*5/1023;          // to store the float value of the ADC
	char charadc[30];                               // to store the character array for the ADC measurement
	sprintf(charadc,"v = %.3f V",adcvalfloat);      // merge the float value and characters into the character array
	usart_prints(charadc);                          // print the ADC measurement (format: 'v = X.XXX V')
	return ADC;
}

void resetDAC(unsigned char channel) // Resets all DAC registers
{
	i2c_start_wait(DevDAC+I2C_WRITE);
	i2c_write(0b00010000+channel); // Send the RESET command
	i2c_stop();
}


void EEPROM_write(unsigned int uiAddress, uint8_t ucData)
{
	/* Wait for completion of previous write */
	while(EECR & (1<<EEPE))	;
	/* Set up address and Data Registers */
	EEAR = uiAddress;
	EEDR = ucData;
	
	/* Write logical one to EEMPE */
	EECR = 0x04;
	/* Start eeprom write by setting EEPE */
	EECR |= (1<<EEPE);
}

uint8_t EEPROM_read(unsigned int uiAddress)
{
	/* Wait for completion of previous write */
	while(EECR & (1<<EEPE));
	/* Set up address register */
	EEAR = uiAddress;
	/* Start eeprom read by writing EERE */
	EECR |= (1<<EERE);
	/* Return data from Data Register */
	return EEDR;
}

void setValue(unsigned char channel,float voltage)
{
	unsigned char d;
	float tmp;
	voltage = voltage + 0.00;
	tmp = (voltage*100)*(255.0);
	tmp = tmp/500.0;
	d = (unsigned char)(tmp+0.5);
	i2c_start_wait(DevDAC+I2C_WRITE); // Issue START and then send the address
	i2c_write(0x00+channel); // Select DAC0 or DAC1
	i2c_write(d); // Write data
	i2c_stop(); // Issue a STOP
}