/*******************************************************************************/
// Date: 9/10/2021
// ADC display on seven segment display
/******************************************************************************/

#include <msp430.h>

//Function Prototypes...
void displaySevenSegment(int);
void inputRead(int);

// Main function of the program...
int main(void)
{
 	WDTCTL = WDTPW | WDTHOLD;	         // stop Watchdog timer
	P2SEL = 0x00;                            // Selecting the 2nd Port for the GPIO
	P2DIR |= 0xFF;                           // Selecting all the PINS as output
	P2OUT = 0x00;                            // Initializing the Port 2 PINS
	P2REN = 0x00;                            // Setting the PULLUP resistor.

	P1DIR = 0x00;                            // Setting input direction.
	P1SEL |= BIT3;                           // Setting P1.0 as analog input.
    ADC10AE0 = 0x01;                             // Select Channel A0.
    ADC10CTL1 = INCH_3  + ADC10DIV_3;            // Setting the registers for
    ADC10CTL0 = ADC10SHT_0 + MSC + ADC10ON;      // ADC channel control...

	while(1)
	{
	    ADC10CTL0 |= ENC + ADC10SC;          // Sampling and conversion start...
	    int rawNumber = ADC10MEM;
	    int voltageDigit = (rawNumber & 0x03FF);
        Read(voltageDigit);
    }
}

/*********************************************************************************
 **************************Function Definition***********************************
 ********************************************************************************/
//Function for the definition of seven segment display...
void displaySevenSegment(int i)
   {
        int num = i;

        switch(num)  {
        case 0 :
            P2OUT = ~(0x3F);
            break;
        case 1 :
            P2OUT = ~(0x06);
            break;
        case 2 :
            P2OUT = ~(0x5b);
            break;
        case 3 :
            P2OUT = ~(0x4F);
            break;
        case 4 :
            P2OUT = ~(0x66);
            break;
        case 5 :
            P2OUT = ~(0x6d);
            break;
        case 6 :
            P2OUT = ~(0x7d);
            break;
        case 7 :
            P2OUT = ~(0x07);
            break;
        case 8 :
            P2OUT = ~(0x7F);
            break;
        case 9 :
            P2OUT = ~(0x6F);
            break;
        case 10 :
            P2OUT = ~(0x77);
            break;
        case 11 :
            P2OUT = ~(0x7c);
            break;
        case 12 :
            P2OUT = ~(0x39);
            break;
        case 13 :
            P2OUT = ~(0x5e);
            break;
        case 14 :
            P2OUT = ~(0x79);
            break;
        case 15 :
            P2OUT = ~(0x71);
            break;
        default:
            P2OUT = ~(0x40);
        }
   };
//Function taking input value and calling the deven segment display...
void inputRead(int value)
   {
        if(value < 64)
            {display7Segment(0);}
        else if((value < 128 ))
            {display7Segment(1);}
        else if((value < 192))
            {display7Segment(2);}
        else if(value < 256 )
            {display7Segment(3);}
        else if(value < 320 )
            {display7Segment(4);}
        else if(value < 384 )
            {display7Segment(5);}
        else if(value < 448 )
            {display7Segment(6);}
        else if(value < 512 )
            {display7Segment(7);}
        else if(value < 576 )
            {display7Segment(8);}
        else if(value < 640 )
            {display7Segment(9);}
        else if(value < 704 )
            {display7Segment(10);}
        else if(value < 768 )
            {display7Segment(11);}
        else if(value < 832 )
            {display7Segment(12);}
        else if(value < 896 )
            {display7Segment(13);}
        else if(value < 960 )
            {display7Segment(14);}
        else
            {display7Segment(15);}
   };
