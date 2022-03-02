/*******************************************************************************/
//Date: 10/08/2021
//Stop watch Timer implementation with GPIO and Timer interrupt.
/******************************************************************************/

// libraries and header files
#include <msp430.h> 

// function prototypes
void displaySevenSegment(int);
void displaySevenSegmentTwo(int);

// global variables
unsigned int i = 0;                   // variable for 0.1 second counter
unsigned int j = 0;                   // variable for second counter
unsigned int flag = 0;                // flag variable for GPIO interrupt
unsigned int Counter;                 // counter variable for timer interrupt

// main function
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;         // stop watchdog timer

    // setup port 2 for seven segment display
    P2SEL = 0x00;
    P2DIR |= 0xFF;
    P2OUT = 0x00;
    P2REN = 0x00;

    // using the port 1 pin 3 for the switch input and other for outputs
    P1SEL = 0x00;
    P1DIR |= 0xFF;                    // setup Port 1
    P1DIR &= ~(BIT3);                 // config P1.3 as an input (SW1)
    P1IE  |= BIT3;                    // P1.3 interrupt enabled
    P1IES |= BIT3;                    // P1.3 Hi/Lo edge
    P1REN |= BIT3;                    // Enable Pull Up on SW2 (P1.3)
    P1IFG &= ~BIT3;                   // P1.3 IFG cleared
                                      // BIT3 on Port 1 can be used as Switch2
    
    // setup the timer interrupts
    BCSCTL1 = CALBC1_1MHZ;                 // set 1MHz clk source
    DCOCTL = CALDCO_1MHZ;
    TACCTL0 |= CCIE;                       // CCR0 interrupt enabled
    TACCR0 = 1000 - 1;                     // init the capture control
    TACTL = TASSEL_2 + MC_2 + ID_0;        // SMCLK, continous mode
    Counter = 0;

    // init seven segment display
    P1OUT = ~(BIT0 + BIT1 + BIT2 + BIT4 + BIT5 + BIT6);
    P2OUT = ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5);

    __enable_interrupt ();


    while(1)
    {
          displaySevenSegment(j);       // display seconds
          if (j == 10)
          { j = 0;}
          displaySevenSegmentTwo(i);    // display i/10th of second
    }
}


// GPIO ISR for stopping and resetting the stop watch
#pragma vector = PORT1_VECTOR
__interrupt void Port_1 (void){
    flag += 1;                          // increment the flag on switch press
    flag %= 3;                          // using the Modulo to make sure range is 0-2
    __delay_cycles(100000);             // delay for switch debouncing
    P1IFG &= ~BIT3;                     // P1.3 IFG cleared
}


//Timer A0 interrupt service routine
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A_CCR0_ISR(void)
{
    if(flag == 0)                       // stopwatch starts
    { Counter++;
       if(Counter >= 2)
       { i++;
           if ( i == 10)
           { j++;}
           i %= 10;
           Counter = 0;
       }
    }
    if(flag == 1)                       // stopwatch is paused
    { i +=0;}
    if(flag == 2)                       // stopwatch is reset
    { i = 0;
      j = 0;
    }
}


// display seconds Function
void displaySevenSegment(int j)
   {
        int num = j;

        switch(num)  {
        case 0 :
            P2OUT = ~(0xbF);
            break;
        case 1 :
            P2OUT = ~(0x86);
            break;
        case 2 :
            P2OUT = ~(0xdb);
            break;
        case 3 :
            P2OUT = ~(0xcF);
            break;
        case 4 :
            P2OUT = ~(0xe6);
            break;
        case 5 :
            P2OUT = ~(0xed);
            break;
        case 6 :
            P2OUT = ~(0xfd);
            break;
        case 7 :
            P2OUT = ~(0x87);
            break;
        case 8 :
            P2OUT = ~(0xfF);
            break;
        case 9 :
            P2OUT = ~(0xeF);
            break;
        default:
            P2OUT = ~(0xc0);
        }
   }

// display deci seconds function
void displaySevenSegmentTwo(int i)
   {
        int num = i;

        switch(num)  {
        case 0 :
            P1OUT = ~(0x77);
            break;
        case 1 :
            P1OUT = ~(0x41);
            break;
        case 2 :
            P1OUT = ~(0xb3);
            break;
        case 3 :
            P1OUT = ~(0x97);
            break;
        case 4 :
            P1OUT = ~(0xc6);
            break;
        case 5 :
            P1OUT = ~(0xd5);
            break;
        case 6 :
            P1OUT = ~(0xF5);
            break;
        case 7 :
            P1OUT = ~(0x07);
            break;
        case 8 :
            P1OUT = ~(0xF7);
            break;
        case 9 :
            P1OUT = ~(0xc7);
            break;
        default:
            P1OUT = ~(0xc0);
        }
   }