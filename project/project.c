/* File name: gradLAb.c
* Author: written by Somto A. - 12/15/2021
* Target board: msp430G2553
* Compiler version: TI v16.9.0.LTS
* Description: This file provides the source code for graduate lab.
* The completed lab will have the following functionalities:
* 1. Read calculate the time of flight of the ultra-sound sensor using the msp430g2553 chip
* 2. There should be a maximum of 4m and minimum of 0.02m
* 3. Create a delay timer using interrupt.
* 4. Print to print the measured distance to the 2 7segment display leds via a buffer.
* 5. A push button should be able to hold the real time measurement for at least 5
seconds.
*
*
*/
//--------------------------------------------------
// msp430g2553 to HC SR04
// Vcc +3.7V (power)
// P1.1 trigger
// P1.2 echo
// GND GND (ground)
//--------------------------------------------------
//--------------------------------------------------
// SN74LS47 Buffer To 7-Segment
// Vcc +3.8V (power)
// a -> a
// .
// .
// .
// g -> g
// GND GND (ground)
//--------------------------------------------------
//--------------------------------------------------
// sn74ls47(1) to msp430
// a buffer -> {msp430 P2.0}
// b buffer -> {msp430 P2.1}
// c buffer -> {msp430 P2.2}
// d buffer -> {msp430 P2.3}
//--------------------------------------------------
//--------------------------------------------------
// sn74ls47(2) to msp430
// a buffer -> {msp430 P2.4}
// b buffer -> {msp430 P2.5}
// c buffer -> {msp430 P2.6}
// d buffer -> {msp430 P2.7}
//--------------------------------------------------
//--------------------------------------------------
// push button to msp430
// {msp430 P1.3}
//--------------------------------------------------



/****************************************************/
//Declare Preprocessor definition
/****************************************************/
#include <msp430.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


// initialize global variables
int miliseconds = 0;
volatile float distance = 0;
long sensor = 0;
int intDist = 0;
int intDistt = 0;
volatile float tmpDist = 0;


/****************************************************/
// function prototype declarations
/****************************************************/
void displaySevenSegment1(int);
void displaySevenSegment2(int);
void buttonHold(void);


/****************************************************/
//Start of main routine
/****************************************************/
/****************************************************/
/* Function: main
* Description: main routine to get distance from the HCSR04
* to any solid surface being pointed at.
* Arguments: void
* Return: 0 on clean exit, integer other than 0 for
* any error
*/
/****************************************************/
void main(void){

    WDTCTL = WDTPW + WDTHOLD;       // stop WDT

    // setup port 2 for 7-Segment display
    P2SEL = 0x00;       // selecting the 2nd Port for the GPIO
    P2DIR |= 0xFF;      // selecting all the pins as output
    P2OUT = 0x00;       // initializing the Port 2 pins
    P2REN = 0x00;       // setting the PULLUP resistor
    
    // configuring real time clock and Timer_A
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;       // submainclock 1mhz
    CCTL0 = CCIE;       // CCR0 interrupt enabled
    CCR0 = 1000;        // 1ms at 1mhz
    TACTL = TASSEL_2 + MC_1;        // SMCLK, upmode

    P1IFG = 0x00;       // clear all interrupt flags
    
    __enable_interrupt();       //enable global interrupt

    while (1){

        // setup port 1 pins interrupts for trigger and echo sequence
        P1IE &= ~0x01;      // disable interupt
        P1DIR |= 0x02;      // trigger pin as output
        P1OUT |= 0x02;      // generate pulse

        __delay_cycles(10);     // for 10us
        P1OUT &= ~0x02;     // stop pulse
        P1DIR &= ~0x04;     // make pin P1.2 input (ECHO)
        P1IFG = 0x00;       // clear flag just in case anything happened before
        P1IE |= 0x04;       // enable interupt on ECHO pin
        P1IES &= ~0x04;     // rising edge on ECHO pin

        __delay_cycles(30000); // delay for 30ms (after this time echo times out if there is no object detected)
        distance = (sensor / 58) * 0.01;        // converting ECHO lenght into cm

        // set measurement boundaries "4.0 > distance >0.02"
        if (distance > 4.0){

            distance = 4.0;

        }

        if (distance < 0.02){

            distance = 0.2;

        }

        // convert measured distance to digital output
        intDist = floor(distance);
        tmpDist = (distance - intDist) * 10;
        tmpDist = floor(tmpDist);
        intDistt = tmpDist;

        // display measured distance on seven segment
        displaySevenSegment1(intDist);      // displays whole number on first led
        __delay_cycles(30000);
        displaySevenSegment2(intDistt);     // displays decimal fraction on second led
        __delay_cycles(300000);

        buttonHold();       // checks hold button status

    }
}



/****************************************************/
/* Function: displaySevenSegment1
* Description: displays the measured distance. This is
* whole number(left side of the decimal).
* Arguments: i
* Return: --
*/
/****************************************************/
void displaySevenSegment1(int i){

    int digit = i;
    switch (digit){

        case 0:

            P2OUT = 0x00;
            break;

        case 1:

            P2OUT = 0x10;
            break;

        case 2:

            P2OUT = 0x20;
            break;

        case 3:

            P2OUT = 0x30;
            break;

        case 4:

            P2OUT = 0x40;
            break;

        case 5:

            P2OUT = 0x50;
            break;

        case 6:
            
            P2OUT = 0x60;
            break;

        case 7:
            
            P2OUT = 0x70;
            break;

        case 8:

            P2OUT = 0x80;
            break;

        case 9:

            P2OUT = 0x90;
            break;

        default:

            P2OUT = 0x00;

    }
}



/****************************************************/
/* Function: displaySevenSegment1
* Description: displays the measured distance. This is
* the number to right side of the decimal.
* Arguments: j
* Return: --
*/
/****************************************************/
void displaySevenSegment2(int j){

    int digit = j;
    switch (digit){

        case 0:

            P2OUT |= 0x00;
            break;

        case 1:

            P2OUT |= 0x01;
            break;

        case 2:

            P2OUT |= 0x02;
            break;

        case 3:

            P2OUT |= 0x03;
            break;

        case 4:

            P2OUT |= 0x04;
            break;

        case 5:

            P2OUT |= 0x05;
            break;

        case 6:

            P2OUT |= 0x06;
            break;

        case 7:

            P2OUT |= 0x07;
            break;

        case 8:

            P2OUT |= 0x08;
            break;

        case 9:
            
            P2OUT |= 0x09;
            break;

        default:

            P2OUT |= 0x00;
    }
}



/****************************************************/
/* Function: buttonHold
* Description: This initializes the P1.3, checks if
* the button connected to P1.3. If connected triggers
* an interrupt.
*
* Arguments: void
* Return: --
*/
/****************************************************/
void buttonHold(void){
    
    __delay_cycles(30000);
    P1DIR &= ~BIT3;
    P1IFG = 0x00;
    P1REN |= BIT3;
    P1IE |= BIT3;
}



//*****************************************************************************
// ISR : PORT1
// This interrupt is triggered from an pin on port1
//*****************************************************************************
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void){

    // if interrupt is triggered by P1.2, converts TOF to meaningful data
    if (P1IFG & 0x04)       //is there interrupt pending?
    {
        if (!(P1IES & 0x04))        // is this the rising edge?
        {

            TACTL |= TACLR;     // clears timer A
            miliseconds = 0;
            P1IES |= 0x04;      //falling edge

        }
        else
        {
            
            sensor = (long) miliseconds * 1000 + (long) TAR;        // calculating ECHO length

        }

        P1IFG &= ~0x04;     // clear flag
    
    }

    // if interrupt is triggered by P1.3, the realtime measurement is displayed
    // on the 7 segment display for ~5 seconds
    if (P1IFG & BIT3)
    {

        displaySevenSegment1(intDist);
        displaySevenSegment2(intDistt);
        __delay_cycles(3000000);
        P1IFG &= ~BIT3;     // clear flag

    }
}



//*******************************************************
// ISR : Timer
//*******************************************************
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A(void)
{

    // counts every millisecond the interrupt is enabled
    miliseconds++;

}