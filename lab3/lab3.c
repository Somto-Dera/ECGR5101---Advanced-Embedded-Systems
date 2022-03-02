/*******************************************************************************/
// Date: 9/24/2021
// Lighting the LEDs using the ADC, DAC, POT and UART communications
/******************************************************************************/

#include <msp430.h> 
#include "PWM.h"
#include "pin_common.h"
#include "UART.h"
#include "DOUT.h"
#include "common.h"
#include "msp430g2553.h"

//function prototypes
void setPwmOut(int);

// main function
void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	              // stop watchdog timer
	P2DIR |= 0xFF;                            // Selecting all the pins as output
	pinSetPWM(GPIO_PORT_P2,GPIO_PIN1);        // Setup the ports and pins for PWM
	uartInit();                               // Initializing and configuration of the UART peripheral
	P1DIR = 0x00;                             // Setup input direction ADC input.
	P1SEL |= BIT3;                            // Setup P1.0 as analog input.
	ADC10AE0 |= 0x01;                         // Select Channel A0.
	ADC10CTL1 = INCH_3  + ADC10DIV_3;         // Setup the registers
	ADC10CTL0 = ADC10SHT_0 + MSC + ADC10ON;   // ADC channel control
	ADC10CTL0 |= ENC + ADC10SC;
	while(1)
	{
         if((P1IN & 0x20) == 0x20)             // Setting the board as transmitter if P1.5 high
         {
            ADC10CTL0 |= ENC + ADC10SC;
            unsigned int rawNumber = ADC10MEM;
            unsigned int dataByte = (rawNumber & 0x00FF);  // Casting ADC data to bytes to send
            uartTransmitChar(dataByte);        // UART Transmission
	        setPwmOut(dataByte);               // PWM output
	     }
         else if((P1IN & 0x20) == 0x00)        // Setting the board as receiver if P1.5 low
	     {                                     // If board is receiver then just use PWM
	        unsigned int rxData = uartReceiveChar();     // Receiving the data on the UART
	        setPwmOut(rxData);                 // Output on PWM
	     }

	}
}

// function for setting the PWM Output
void setPwmOut(int data)
{
    if (data == 0)
       {pwmSetDuty(0);}
    if (data > 0   &&  data <= 25 )         // if transfered data is > 0    & <= 25   dutyCycle 10 %
       { pwmSetDuty(10);}
    if (data > 25  &&  data <= 50 )         // if transfered data is > 25   & <= 50   dutyCycle 20 %
        { pwmSetDuty(20);}
    if (data > 50  &&  data <= 75 )         // if transfered data is > 50   & <= 75   dutyCycle 30 %
       { pwmSetDuty(30);}
    if (data > 75  && data <= 100 )         // if transfered data is > 75   & <= 100  dutyCycle 40 %
       { pwmSetDuty(40);}
    if (data > 100 && data <= 125 )         // if transfered data is > 100  & <= 125  dutyCycle 50 %
       { pwmSetDuty(50);}
    if (data > 125 && data <= 150 )         // if transfered data is > 125  & <= 150  dutyCycle 60 %
       { pwmSetDuty(60);}
    if (data > 150 && data <= 175 )         // if transfered data is > 150  & <= 175  dutyCycle 70 %
       { pwmSetDuty(70);}
    if (data > 175 && data <= 200 )         // if transfered data is > 175  & <= 200  dutyCycle 80 %
       { pwmSetDuty(80);}
    if (data > 200 && data <= 225 )         // if transfered data is > 200  & <= 225  dutyCycle 90 %
       { pwmSetDuty(90);}
    if (data > 225 && data <= 255 )         // if transfered data is > 225  & <= 255  dutyCycle 100 %
       { pwmSetDuty(100);}
}