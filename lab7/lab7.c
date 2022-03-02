/* File name: LabSeven.c
 * Author: template provided by Karim Erian  - 11/17/2021
 *         modified by Somto A. and Harshil Suthar  - 11/20/2021
 * Target board: TIVA C: Tiva TM4C123GH6PM
 * Compiler version: TI v16.9.0.LTS
 * Description: This file provides the template for LabSeven.
 * The completed lab will have the following functionalities:
 * 1. Read eduboostpack mk II accelerometer data using Tiva TM4C123GH6PM adc pins.
 * 2. Create a delay timer using interrupt.
 * 3. Print to eduboostpack mk II lcd.
 * 4. Print a 3x3 blob to eduboostpack mk II lcd and move blob pixel by pixel based on
 *    accelerometer ADC value at a rate of 50millisecs.
 *
 * Src code link: https://drive.google.com/drive/folders/1uzd8fOZhE_zJRqQAXGgVwD4hH0tCUT0u?usp=sharing
 *
*/

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
#include <stdint.h>     // c standard header
#include <stdbool.h>    // c standard boolean header
#include <stdio.h>
#include "tm4c123gh6pm.h"  //  ADC driver header
#include "buttons.h"
#include "string.h"
#include "ST7735.h"
#include "PLL.h"
#include "TM4C123.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"      // device memory map header
#include "driverlib/sysctl.h"   // system control driver header
#include "driverlib/gpio.h"     // GPIO API header
#include "driverlib/pin_map.h"  // pin-to-peripheral mapping header
#include "driverlib/pwm.h"  //  PWM driver header
#include "driverlib/adc.h"  //  ADC driver header
#include "driverlib/ADC.h"  //  ADC driver header
#include "driverlib/debug.h"  //  ADC driver header
#include "driverlib/adc.h"  //  ADC driver header
#include "driverlib/interrupt.h"  //  ADC driver header
#include "driverlib/timer.h"  //  ADC driver header

// define lcd dimensions
#define LCDHEIGHT 128
#define LCDWIDTH 128

// lcd refresh rate
#define FLIP_FREQ 4

// define pixel boundaries
#define MAX_X 2140
#define MAX_Y 2175
#define MIN_X 1880
#define MIN_Y 1915

// initialize global variables for tracking and
// moving blob
int x = 0;      // rt blob pixel location
int y = 0;
int init_x = 0;     // initial blob pixel location variable
int init_y = 0;
int final_x = 0;    // target blob pixel location variable
int final_y = 0;
int delta_x = 0;    // determine rate of blob pixel move
int delta_y = 0;
uint32_t ui32ADC0Value[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };



/****************************************************/
// function prototype declarations
/****************************************************/
void circle(int x, int y);
void delayMs(uint32_t ui32Ms);
void initADC();
void readADC();
void initMisc();
void initLCD();
void blobMove();



const uint16_t Circle_Dia_3[] = { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
                                  0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF,
                                  0XFFFF, 0x0000, 0x0000, 0x0000, 0xFFFF,
                                  0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF,
                                  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF };

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
//Start of main routine
/****************************************************/

/****************************************************/
/* Function: main
 * Description: main routine to get accelerometer val
 * and move blob on lcd in rt
 * Arguments: void
 * Return: 0 on clean exit, integer other than 0 for
 * any error
 */
/****************************************************/
int main()
{

    int flip = FLIP_FREQ;;

    initMisc();

    initLCD();
    initADC();

    //Flush Screen
    ST7735_FillScreen(0xFFFF);

    // init variables to center of lcd
    init_x = LCDHEIGHT / 2;
    init_y = LCDWIDTH / 2;

    // init variables to "1" inorder to make blob
    // movement continuous
    delta_x = 1;
    delta_y = 1;


    // init x, final_x to init_x
    x = init_x;
    y = init_y;
    final_x = init_x;
    final_y = init_y;

    // draw ball at center
    circle(x, y);

    while (1)
    {
        if (flip == 0)
        {
            ST7735_FillScreen(0xFFFF);
            flip = FLIP_FREQ;
        }

        flip--;

        // call readADC function to read ADC sensor
        readADC();

        // call blobMove to control blob movement
        blobMove();

    }

    return 0;

}


/****************************************************/
/* Function: circle
 * Description: draws blob to lcd based on input x and y
 * vals.
 * Arguments: x , y
 * Return: --
 */
/****************************************************/
void circle(int x, int y)
{

    ST7735_DrawBitmap(x, y, Circle_Dia_3, 5, 5);

}


/****************************************************/
/* Function: delayMs
 * Description: delay for certain milliseconds based on
 * input ui32Ms.
 * Arguments: ui32Ms
 * Return: --
 */
/****************************************************/
void delayMs(uint32_t ui32Ms)
{

    // 1 clock cycle = 1 / SysCtlClockGet() second
    // 1 SysCtlDelay = 3 clock cycle = 3 / SysCtlClockGet() second
    // 1 second = SysCtlClockGet() / 3
    // 0.001 second = 1 ms = SysCtlClockGet() / (3 * 1000)
    //compute the period of interrupts generated
    SysCtlDelay(ui32Ms * (SysCtlClockGet() / (3 * 1000)));

}


/****************************************************/
/* Function: readADC
 * Description: read adc accelerometer value and stores
 * in global variable "ui32ADC0Value"
 * Arguments: void
 * Return: --
 */
/****************************************************/
void readADC()
{

    // Clear the ADC interrupt flag.
//    ADCIntClear(ADC0_BASE, 0);

    // Trigger the ADC conversion.
    ADCProcessorTrigger(ADC0_BASE, 0);

    // Wait for conversion to be completed.
    while (!ADCIntStatus(ADC0_BASE, 0, 0))
    { puts("Waiting for adc data"); }
    ADCIntClear(ADC0_BASE, 0);
    // Read ADC Value. ADC has 12-bit precision so the output ranges from
    // 0 to 4095
    ADCSequenceDataGet(ADC0_BASE, 0, ui32ADC0Value);

}


/****************************************************/
/* Function: initADC
 * Description: initialize and configure ADC and
 * peripherals
 * Arguments: void
 * Return: --
 */
/****************************************************/
void initADC()
{

    // Set system clock
    SysCtlClockSet(
            SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ
                    | SYSCTL_OSC_MAIN);

    // Enable the ADC peripheral and wait for it to be ready.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0))
    { puts("enabling ADC peripheral"); };
    // Enable GPIO port D and wait for it to be ready.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD)){ puts("enabling gpio port"); }

    // Configure PD0,PD1 as an ADC input
    GPIOPinTypeADC(GPIO_PORTD_BASE, 0x03);

    // Configure ADC0 sequencer and enable it
    ADCSequenceDisable(ADC0_BASE, 0);
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH7);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 1,
                             ADC_CTL_CH6 | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 0);

}


/****************************************************/
/* Function: initMisc
 * Description: set sys clock.
 * Arguments: void
 * Return: --
 */
/****************************************************/
void initMisc()
{

    //Set system clock to 80 MHz
    PLL_Init(Bus80MHz);
    ST7735_InitR(INITR_REDTAB);

}


/****************************************************/
/* Function: blobMove
 * Description: uses adc data to control the blob
 * pose on lcd in rt
 * Arguments: void
 * Return: --
 */
/****************************************************/
void blobMove()
{

    int i = 0;
    int j = 0;
    int r = 0;
    int q = 0;
    int slope = 0;

    if (ui32ADC0Value[0] > MAX_X - 20)
    {
        ui32ADC0Value[0] = MAX_X - 20;

    }
    else if (ui32ADC0Value[0] < MIN_X)
    {
        ui32ADC0Value[0] = MIN_X + 20;

    }
    if (ui32ADC0Value[1] > MAX_Y - 20)
    {
        ui32ADC0Value[1] = MAX_Y - 20;
    }
    else if (ui32ADC0Value[1] < MIN_Y + 20)
    {
        ui32ADC0Value[1] = MIN_Y + 20;
    }
    final_x = (ui32ADC0Value[0] - MIN_X) / 2;
    final_y = (ui32ADC0Value[1] - MIN_Y) / 2;
    final_y = 128 - final_y;

    if ((final_x == init_x) && (final_y == init_y))
    {
        circle(x, y);

    }
    else if (final_x == init_x)
    {
        if (final_y > init_y)
        {
            r = final_y - y;
            delta_y = 1;
            for (i = 0; i < r; i++)
            {
                y = y + delta_y;
                circle(x, y);
                delayMs(50);

            }
        }
        else
        {
            delta_y = -1;
            r = y - final_y;
            for (i = 0; i < r; i++)
            {
                y = y + delta_y;
                circle(x, y);
                delayMs(50);

            }
        }
    }
    else if (final_y == init_y)
    {
        if (final_x > init_x)
        {
            delta_x = 1;
            r = final_x - x;
            for (i = 0; i < r; i++)
            {
                x = x + delta_x;
                circle(x, y);
                delayMs(50);
            }
        }
        else
        {
            delta_x = -1;
            r = x - final_x;
            for (i = 0; i < r; i++)
            {
                x = x - delta_x;
                circle(x, y);
                delayMs(50);
            }
        }
    }

    // Conditions control blob movement based adc value
    // when final_x != init_x && final_y != init_y
    else if (final_x > init_x)
    {
        delta_x = 1;
        if (final_y > init_y)
        {

            delta_y = 1;

            if ((final_x - init_x) > (final_y - init_y))
            {
                r = final_y - y;
                slope = (int) ((final_x - init_x) / (final_y - init_y));
                for (i = 0; i < r; i++)
                {
                    y = y + delta_y;
                    for (j = 0; j < slope; j++)
                    {

                        x = x + delta_x;
                        circle(x, y);
                        delayMs(50);
                    }
                }
                q = (final_x - init_x) - (slope * (final_y - init_y));
                for (i = 0; i < q; i++)
                {
                    x = x + delta_x;
                    circle(x, y);
                    delayMs(50);
                }
            }
            else
            {
                r = final_x - init_x;
                slope = (int) ((final_y - init_y) / (final_x - init_x));
                for (i = 0; i < r; i++)
                {
                    x = x + delta_x;
                    for (j = 0; j < slope; j++)
                    {

                        y = y + delta_y;
                        circle(x, y);
                        delayMs(50);
                    }
                }
                q = (final_y - init_y) - (slope * (final_x - init_x));
                for (i = 0; i < q; i++)
                {
                    y = y + delta_y;
                    circle(x, y);
                    delayMs(50);
                }
            }
        }
        else
        {
            delta_y = -1;

            if ((final_x - init_x) > (init_y - final_y))
            {
                r = init_y - final_y;
                slope = (int) ((final_x - init_x) / (init_y - final_y));
                for (i = 0; i < r; i++)
                {

                    y = y + delta_y;
                    for (j = 0; j < slope; j++)
                    {

                        x = x + delta_x;
                        circle(x, y);
                        delayMs(50);
                    }
                }
                q = (final_x - init_x) - (slope * (init_y - final_y));
                for (i = 0; i < q; i++)
                {
                    x = x + delta_x;
                    circle(x, y);
                    delayMs(50);
                }
            }
            else
            {
                r = final_x - init_x;
                slope = (int) ((init_y - final_y) / (final_x - init_x));
                for (i = 0; i < r; i++)
                {
                    x = x + delta_x;
                    for (j = 0; j < slope; j++)
                    {

                        y = y + delta_y;
                        circle(x, y);
                        delayMs(50);
                    }
                }
                q = (init_y - final_y) - (slope * (final_x - init_x));
                for (i = 0; i < q; i++)
                {
                    y = y + delta_y;
                    circle(x, y);
                    delayMs(50);
                }
            }
        }
    }
    else
    {
        delta_x = -1;
        if (final_y > init_y)
        {
            delta_y = 1;
            if ((init_x - final_x) > (final_y - init_y))
            {
                r = final_y - init_y;
                slope = (int) ((init_x - final_x) / (final_y - init_y));
                for (i = 0; i < r; i++)
                {
                    y = y + delta_y;
                    for (j = 0; j < slope; j++)
                    {
                        x = x + delta_x;
                        circle(x, y);
                        delayMs(50);

                    }
                }
                q = (init_x - final_x) - (slope * (final_y - init_y));
                for (i = 0; i < q; i++)
                {
                    x = x + delta_x;
                    circle(x, y);
                    delayMs(50);

                }
            }
            //else if ((init_x - final_x) < (final_y - init_y))
            else
            {
                r = init_x - final_x;
                slope = (int) ((init_x - final_x) / (final_y - init_y));
                for (i = 0; i < r; i++)
                {
                    x = x + delta_x;
                    for (j = 0; j < slope; j++)
                    {
                        y = y + delta_y;
                        circle(x, y);
                        delayMs(50);
                    }

                }
                q = (init_x - final_x) - (slope * (final_y - init_y));
                for (i = 0; i < q; i++)
                {
                    y = y + delta_y;
                    circle(x, y);
                    delayMs(50);
                }
            }
        }
        else
        {

            delta_y = -1;
            if ((init_x - final_x) > (init_y - final_y))
            {
                r = init_y - final_y;
                slope = (int) ((init_x - final_x) / (init_y - final_y));
                //for (i = 0; i < (difference[1] * -1); i++)
                //for (i = 0; i < (0 * -1); i++)
                for (i = 0; i < r; i++)
                {
                    y = y + delta_y;
                    for (j = 0; j < slope; j++)
                    {
                        x = x + delta_x;
                        circle(x, y);
                        delayMs(50);
                    }
                }
                q = (init_x - final_x) - (slope * (init_y - final_y));
                for (i = 0; i < q; i++)
                {
                    x = x + delta_x;
                    circle(x, y);
                    delayMs(50);

                }
            }
            else
            {
                r = init_x - final_x;
                slope = (int) ((init_y - final_y) / (init_x - final_x));
                for (i = 0; i < r; i++)
                {
                    x = x + delta_x;
                    for (j = 0; j < slope; j++)
                    {

                        y = y + delta_y;
                        circle(x, y);
                        delayMs(50);

                    }
                }
                q = (init_y - final_y) - (slope * (init_x - final_x));
                for (i = 0; i < q; i++)
                {

                    y = y + delta_y;
                    circle(x, y);
                    delayMs(50);

                }
            }
        }
    }

    init_x = x;
    init_y = y;

}


/****************************************************/
/* Function: initLCD
 * Description: initialize lcd and print i and my
 * partners name
 * Arguments: void
 * Return: --
 */
/****************************************************/
void initLCD()
{

    int i = 0;
    int j = 0;

    while (i < 1)
    {
        i++;
        ST7735_DrawCharS(18, 2, 'L', ST7735_Color565(255, 0, 0), 0, 3);
        ST7735_DrawCharS(36, 2, 'A', ST7735_Color565(255, 0, 0), 0, 3);
        ST7735_DrawCharS(54, 2, 'B', ST7735_Color565(255, 0, 0), 0, 3);
        ST7735_DrawCharS(72, 2, '-', ST7735_Color565(255, 0, 0), 0, 3);
        ST7735_DrawCharS(90, 2, '7', ST7735_Color565(255, 0, 0), 0, 3);
        for (j = 0; j <= 112; j = j + 14)
        {
            ST7735_DrawCharS(j, 25, '-', ST7735_Color565(255, 0, 0), 0, 2);
        }
        ST7735_DrawCharS(14, 45, 'H', ST7735_Color565(255, 0, 0), 0, 2);
        ST7735_DrawCharS(28, 45, 'A', ST7735_Color565(255, 0, 0), 0, 2);
        ST7735_DrawCharS(42, 45, 'R', ST7735_Color565(255, 0, 0), 0, 2);
        ST7735_DrawCharS(56, 45, 'S', ST7735_Color565(255, 0, 0), 0, 2);
        ST7735_DrawCharS(70, 45, 'H', ST7735_Color565(255, 0, 0), 0, 2);
        ST7735_DrawCharS(84, 45, 'I', ST7735_Color565(255, 0, 0), 0, 2);
        ST7735_DrawCharS(98, 45, 'L', ST7735_Color565(255, 0, 0), 0, 2);
        ST7735_DrawCharS(54, 69, '&', ST7735_Color565(255, 0, 0), 0, 3);
        ST7735_DrawCharS(28, 98, 'S', ST7735_Color565(255, 0, 0), 0, 2);
        ST7735_DrawCharS(42, 98, 'O', ST7735_Color565(255, 0, 0), 0, 2);
        ST7735_DrawCharS(56, 98, 'M', ST7735_Color565(255, 0, 0), 0, 2);
        ST7735_DrawCharS(70, 98, 'T', ST7735_Color565(255, 0, 0), 0, 2);
        ST7735_DrawCharS(84, 98, '0', ST7735_Color565(255, 0, 0), 0, 2);

        delayMs(1000);
        ST7735_FillScreen(0x0000);
        delayMs(500);
    }
}