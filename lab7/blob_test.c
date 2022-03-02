/*
 * ----------------------------------------------------------------------------------------------------------------------------------------------------------------
 * File: Blob Move
 * Name: Somto Anyaegbu
 * Date: 2021/11/23
 *
 * Summary:
 *  prints blob to screen. blob should move from left to right and vice versa.
 *
 *
 */

// include libraries
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
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

// define lcd length and breadth
#define LCDHEIGHT 128
#define LCDWIDTH 128

int x = 0;
int y = 0;

// Rate of LCD cleaning....
#define FLIP_FREQ 4

// declare function prototype
void circle(int x, int y);
//void DelayWait10ms(uint32_t n);
void delayMs(uint32_t ui32Ms);

//White -> 0xFFFF ;
//Black -> 0x0000;

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

int main()
{

    int flip = FLIP_FREQ;

    int i = 0;
    int j = 0;

    PLL_Init(Bus80MHz);
    ST7735_InitR(INITR_REDTAB);

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

    SysCtlClockSet(
    SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
    // ADC set up...
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    GPIOPinTypeADC(GPIO_PORTD_BASE, 0x03);
    ADCSequenceDisable(ADC0_BASE, 0);
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0); // ADC0 Module, sequencer 0, for 0 sampling, Input is from channel 7 PD0...
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH7); // ADC0 Module, sequencer 0, for 1 sampling, Input is from channel 6 PD1...
    ADCSequenceStepConfigure(ADC0_BASE, 0, 1,
    ADC_CTL_CH6 | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 0);

    //fill screen
    ST7735_FillScreen(0xFFFF);

    unsigned int xi = LCDHEIGHT / 2;
    unsigned int yi = LCDWIDTH / 2;

    x = xi;
    y = yi;
    circle(x, y);

    while (1)
    {
        //
        if (flip == 0)
        {
            ST7735_FillScreen(0xFFFF);
            flip = FLIP_FREQ;
        }

        flip--;

        do
        {

            x = x - 1;
            circle(x, y);
            delayMs(50);

        }
        while (x > 2);

        do
        {

            x = x + 1;
            circle(x, y);
            delayMs(50);

        }
        while (x < 125);

    }
    return 0;
}

void circle(int x, int y)
{
    ST7735_DrawBitmap(x, y, Circle_Dia_3, 5, 5);
}

void delayMs(uint32_t ui32Ms)
{
    // 1 clock cycle = 1 / SysCtlClockGet() second
    // 1 SysCtlDelay = 3 clock cycle = 3 / SysCtlClockGet() second
    // 1 second = SysCtlClockGet() / 3
    // 0.001 second = 1 ms = SysCtlClockGet() / (3 * 1000)
    SysCtlDelay(ui32Ms * (SysCtlClockGet() / (3 * 1000)));
}

