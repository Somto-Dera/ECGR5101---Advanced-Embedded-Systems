/*******************************************************************************/
//Creation Date: 12/09/2021
//Description of Lab09:
//Creating the double player Pong game! Using Tiva & booster board...
//Code for controlling paddle and ball on LCD...
/******************************************************************************/
//--------------------------------------------------
// Connected pins in logic order
// power and reset
// J1.1 +3.3V (power)
// J3.21 +5V (power)
// J3.22 GND (ground)
// J2.20 GND (ground)
// J2.16 nothing (reset)
//--------------------------------------------------
//--------------------------------------------------
// accelerometer
// J3.23 accelerometer X (analog)        {TM4C123 PD0/AIN7}
// J3.24 accelerometer Y (analog)        {TM4C123 PD1/AIN6}
// J3.25 accelerometer Z (analog)        {TM4C123 PD2/AIN5}
//--------------------------------------------------
//--------------------------------------------------
// LCD graphics
// J1.7 LCD SPI clock (SPI)              {TM4C123 PB4}
// J2.13 LCD SPI CS (SPI)                {TM4C123 PA4}
// J2.15 LCD SPI data (SPI)              {TM4C123 PB7}
// J2.17 LCD !RST (digital)              {TM4C123 PF0}
// J4.31 LCD RS (digital)                {TM4C123 PF4}
//
// White -> 0xFFFF
// Black -> 0x0000
//--------------------------------------------------
/****************************************************/
//Declare Preprocessor definition
/****************************************************/

// include libraries
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "PLL.h"
#include "TM4C123.h"
#include "random.h"
#include "driverlib/uart.h"
//#include "utils/uartstdio.h"
#include "uartstdio.h"

// Defination of Macros
#define xMax 125
#define xMin 2
#define yMax 125
#define yMin 2
#define pad_width 4
#define pad_height 17
#define pad_x 2
#define playerOne_x 2
#define playerTwo_x 123
#define LCDHEIGHT 128
#define LCDWIDTH 128
#define XLIMITRIGHT 125
#define XLIMITLEFT 2
#define YLIMITBOTTOM 125
#define YLIMITTOP 2
#define MIN_VALUE 1900
#define MAX_VALUE 2100
#define FLIP_FREQ 4  // Rate of LCD cleaning

// Function prototypes
void circle(int x_pos, int y_pos);
void DelayWait10ms(uint32_t n);
int paddleOneControl(uint32_t, int);
//int paddleTwoControl(uint32_t, int);
int paddleTwoControl(int);
void ConfigureUART(void);

// Global variable declaration...
int playerOne_y = 64;
int playerTwo_y = 64;
int c = 0;
int pad_top = 0;
int player_one_edge = pad_width + playerOne_x;
int player_two_edge = playerTwo_x - pad_width;
volatile char paddleTwo;
volatile char TX_BUFFER = ' ';
uint32_t ui32ADC1Value[8];
// Cycle defination...
const uint16_t Circle_Dia_3[] = { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
                                  0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF,
                                  0XFFFF, 0x0000, 0x0000, 0x0000, 0xFFFF,
                                  0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF,
                                  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF };

// Paddle defination ....
const uint16_t Paddle[] = {

0xFFFF,
                            0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000,
                            0xFFFF, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0xFFFF,
                            0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000,
                            0xFFFF, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0xFFFF,
                            0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000,
                            0xFFFF, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0xFFFF,
                            0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000,
                            0xFFFF, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0xFFFF,
                            0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000,
                            0xFFFF, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0xFFFF,
                            0x0000, 0x0000, 0xFFFF, 0XFFFF, 0xFFFF, 0xFFFF,
                            0xFFFF };

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

/****************************************************/
/* Function: main

 * Description: main routine to execute the one player
 pong game. If ball pass is missed by player/paddle it
 restarts on its own.

 * Arguments: void

 * Return: 0 on clean exit, integer other than 0 for
 any error
 */
/****************************************************/

int main()
{
    // local variables...
    int i = 0;
    int x = 2;
    int y = 3;
    int theta = 0;

    paddleTwo = 'U';
    int flip = FLIP_FREQ;
    PLL_Init(Bus80MHz);
    ST7735_InitR(INITR_REDTAB);

    // Clk, GPIO and ADC Peripheral enable...
    SysCtlClockSet(
            SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ
                    | SYSCTL_OSC_MAIN);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);        // ADC Module 0 Enable...
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);       // GPIO Port D Enable....

    ConfigureUART();               // Function call for configuration of UART...

    //ADC module Configuration...
    ADCSequenceDisable(ADC0_BASE, 3);  // Sequencer disable for configuration...
    ADCReferenceSet(ADC0_BASE, ADC_REF_INT); // Setting ADC reference voltage...
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0); // Trigger Configuration...
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0,
    ADC_CTL_CH4 | ADC_CTL_IE | ADC_CTL_END);                // Setting step 0...
    ADCSequenceEnable(ADC0_BASE, 3);                      // Sequencer enable...
    ADCIntEnable(ADC0_BASE, 3);                           // Interrupt enable...

    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_3);      // Analog input channel...
    ST7735_FillScreen(0xFFFF);         // Filling screen with white functions...
    uint32_t ui32ADC0Value[0];               // Reading the conversion result...

    while (1)
    {
        // Variable initialization...
        float m = 0;
        int xi = 59;
        int yi = 62;
        int dx = -1;
        int dy = 1;
        int y_diff = 0;

        x = xi;
        y = yi;
        // Calculating the slope...
        m = rand() % 2;

        if (c != 0)
        {

            dy *= -1;

        }

        y = (rand() % (120 - 5 + 1)) + 5;

        circle(x, y);

        ADCIntClear(ADC0_BASE, 3);

        while (1)
        {

            ADCProcessorTrigger(ADC0_BASE, 3);

            while (!ADCIntStatus(ADC0_BASE, 3, false))
            {
            }

            ADCSequenceDataGet(ADC0_BASE, 3, ui32ADC0Value);

            playerOne_y = paddleOneControl(ui32ADC0Value[0], playerOne_y);
            playerTwo_y = paddleTwoControl(playerTwo_y);

            if (y >= yMax || y <= yMin)
            {

                dy *= -1;
                y += dy;
                circle(x, y);

            }

            if (x <= player_one_edge && y > playerOne_y - pad_height
                    && y < playerOne_y)
            {

                dx *= -1;
                x += dx;
                circle(x, y);

            }

            if (x >= player_two_edge && y > playerTwo_y - pad_height
                    && y < playerTwo_y)
            {

                dx *= -1;
                x += dx;
                circle(x, y);

            }

            if (x <= xMin || x >= xMax)
            {

                break;

            }

            x += dx;
            //y_diff = (m * x) + c;

            for (i = 0; i < m; i++)
            //for (i = 0; i < y_diff; i++)
            {

                if (y >= xMax || y <= xMin)
                {
                    break;
                }

                y += dy;
                circle(x, y);
                DelayWait10ms(5);

            }

            xi = x;
            yi = y;

            DelayWait10ms(5);

        }

        dx *= -1;

        //clear screen
        ST7735_FillScreen(0xFFFF);
        DelayWait10ms(200);
    }

    return 0;

}

/****************************************************/
/* Function: circle

 * Description: Takes XY-Co-ordinates as input and draws
   the circle on LCD screen.

 * Arguments: int x, int y

 * Return: void
 */
/****************************************************/
void circle(int x_pos, int y_pos)
{
    ST7735_DrawBitmap(x_pos, y_pos, Circle_Dia_3, 5, 5);
}

****************************************************/
/* Function: DelayWait10ms

 * Description: delays program by argumented amount
   of 10ms

 * Arguments: uint32_t n

 * Return: void
 */
/****************************************************/
void DelayWait10ms(uint32_t n)
{
    uint32_t volatile time;
    while (n)
    {
        time = 727240 * 2 / 91;
        while (time)
        {
            time--;
        }
        n--;
    }
}
;

/****************************************************/
/* Function: paddleControl

 * Description: Takes ADC converted result and Y co-ordinates
   of paddle as input and controls the movement of circle on
   LCD screen.

 * Arguments: uint32_t ui32ADC_results, int yPtr

 * Return: int
 */
/****************************************************/
int paddleOneControl(uint32_t ui32ADC_results, int y_pose)
{
    if ((ui32ADC_results < 1900) && (y_pose <= 128))
    {
        y_pose++;
    }
    if ((ui32ADC_results > 2100) && (y_pose >= 16))
    {
        y_pose--;
    }
    ST7735_DrawBitmap(playerOne_x, y_pose, Paddle, pad_width, pad_height);
    return y_pose;
}

/****************************************************/
/* Function: paddleTwoControl

 * Description: Takes ADC converted result and Y co-ordinates
   of paddle as input from other board controls the movement of
   circle on LCD screen.

 * Arguments: uint32_t ui32ADC_results, int yPtr

 * Return: int
 */
/****************************************************/
int paddleTwoControl(int y_pose)
{

    TX_BUFFER = UARTCharGet(UART1_BASE);
    // Received character is U move paddle2 up...
    if (TX_BUFFER == 'U')
    {

        y_pose++;
        if (y_pose >= yMax)
        {
            y_pose--;
        }
    }
    // Received character is F move paddle2 Down...
    else if (TX_BUFFER == 'F')
    {
        y_pose--;
        if (y_pose <= yMin)
        {
            y_pose++;
        }
    }
    // Received character is S keeps paddle2 at same position...
    else
    {
        y_pose = y_pose;
    };

    ST7735_DrawBitmap(playerTwo_x, y_pose, Paddle, pad_width, pad_height);
    return y_pose;

}

/****************************************************/
/* Function: ConfigureUART()

 * Description: Sets up the UART peripherial and setting
   appropriate parameters

 * Arguments: void

 * Return: void
 */
/****************************************************/

void ConfigureUART(void)
{

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinConfigure(GPIO_PB0_U1RX);
    GPIOPinConfigure(GPIO_PB1_U1TX);
    GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(
            UART1_BASE, SysCtlClockGet(), 115200,
            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    UARTEnable(UART1_BASE);
}
