#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h> 
#include <string.h> 
#include "RFM73.h"  

//set Baude rate for debug UART
#define BAUD_RATE 9600

//=======FUNCTION DECLARATIONS==========
void ioinit(void);
void power_on_delay(void);
void uart_putstr(const char *str);
void USARTinit();
void compare(char *);

extern void RFM73_Initialize(void);
extern void SwitchToTxMode(void);
extern void SwitchToRxMode(void);

uint8_t tx_buf[MAX_PACKET_LEN]; 
uint8_t rx_buf[MAX_PACKET_LEN];

extern const uint8_t RX0_Address[];
extern const unsigned long Bank1_Reg0_13[];

//uint8_t txFlag=OFF;
volatile uint8_t rxFlag=OFF;
//======================================


int main (void)
{
  	DDRC = 0x00;
	ioinit();	// initialize the MCU		
	tx_buf[0] = '\0';	// initialize tx buffer

	power_on_delay();  
	
	RFM73_Initialize();
#ifdef Debugg
	USARTinit();
	uart_putstr("**********Welcome to EWS_RFM73_CHAT_EXAMPLE!!!!!**********\n\r\n\r");
#endif	
    while(1) 
	{                   
		
		if (rxFlag == ON)
		{
			rxFlag=OFF;
			Receive_Packet();
		}
		//your code here
	}
	
}

/**************************************************
Function: uart_putstr
Description:
	prints string to UART Port
Parameter:
	Constant pointer of string
Return:
	None
**************************************************/
void uart_putstr(const char *str)	// send a character string on USART1
{
	unsigned char i;
	i = 0;
	
	while (str[i] != '\0') 
	{
		while ( !( UCSRA & (1<<UDRE)) );	// wait for the Tx data buffer to be empty
		UDR = str[i];	// send next character in buffer t
		i++;			
	}
}

/**************************************************
Function: USARTinit
Description:
	Initialize UART
Parameter:
	None
Return:
	None
**************************************************/
void USARTinit()
{
	UBRRH = (((F_CPU/BAUD_RATE)/16)-1)>>8;		// set baud rate
  	UBRRL = (((F_CPU/BAUD_RATE)/16)-1);


	/*Set Frame Format
	
	Asynchronous mode
	No Parity
	1 StopBit
	char size 8

	*/

	UCSRB |= (1 << RXEN) | (1 << TXEN);   // Turn on the transmission and reception circuitry

   	UCSRC |= (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1); // Use 8-bit character sizes


	/*Enable Interrupts
	RXCIE- Receive complete
	UDRIE- Data register empty

	Enable The recevier and transmitter

	*/

	UCSRB |= (1 << RXCIE); // Enable the USART Recieve Complete interrupt (USART_RXC)
   	sei(); // Enable the Global Interrupt Enable flag so that interrupts can be processed
}

/*********************************************************
Function:      power_on_delay()                                    
                                                            
Description:                                                
 
*********************************************************/
void power_on_delay(void)
{
	_delay_ms(1000);
}

/************************************************************************
	FUNCTION: ioinit() 
	DESCRIPTION:  This function prepares the ATMEGA for use: 
		1. Initialize on I/O ports
		2. Initialize SPI port
		3. Initialize UART1 port
*************************************************************************/
void ioinit(void)
{
	
	// Initialize I/O ports
    //1 = output, 0 = input
    DDRB |= (1<<4)|(1<<5)|(1<<7);	// SCK, MOSI and SS as outputs
    DDRB &= ~(1<<6);               	// MISO as input
	DDRB |= (1<<3);					// PB3 is used as an output for chip enable of RFM73 module
	DDRB |= (1<<0);					//GREEN LED
	DDRB |= (1<<1);					//RED LED
	DDRA |= (1<<0);
	
	// Initialize SPI port
    SPCR |= (1<<MSTR);               // Set as Master
    SPCR |= (1<<SPR0)|(1<<SPR1);				// divided clock by 128
    SPCR |= (1<<SPE);                // Enable SPI

	sbi(PORTB,RED_LED);
	sbi(PORTB,GREEN_LED);
	
	//Interrupt Initialization for IRQ
	cbi(PORTD,IRQ);
	
	GICR = 1<<INT0;					// Enable INT0
	//MCUCR = 1<<ISC01 | 1<<ISC00;	// Trigger INT0 on rising edge
	MCUCR = 1<<ISC01;	// Trigger INT0 on falling edge
	
	sei();				//Enable Global Interrupt

}
/**************************************************
Function: compare
Description:
	Compare message 
Parameter:
	char pointer of message
Return:
	None
**************************************************/
void compare(char *string)
{
		//your logic to compare received message
}

ISR(INT0_vect)
{	
	//Set RXFLAG to read message.
	rxFlag = ON;
}