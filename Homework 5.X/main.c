#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<stdio.h>
#include "ws2812b.h"

wsColor colr[4]; // array of color LEDs
float update = .4;
float hue1 = 80; // deciding my colors
float hue2 = 150;
float hue3 = 220;
float hue4 = 290;
float sat = 1;
float bright = 1;

// DEVCFG0
#pragma config DEBUG = OFF // disable debugging
#pragma config JTAGEN = OFF // disable jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // disable flash write protect
#pragma config BWP = OFF // disable boot write protect
#pragma config CP = OFF // disable code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // disable secondary oscillator
#pragma config IESO = OFF // disable switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // disable clock output
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // disable clock switch and FSCM
#pragma config WDTPS = PS1048576 // use largest wdt
#pragma config WINDIS = OFF // use non-window mode wdt
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations


int main() {
    __builtin_disable_interrupts();
    ws2812b_setup();
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);
    BMXCONbits.BMXWSDRM = 0x0;
    INTCONbits.MVEC = 0x1;
    DDPCONbits.JTAGEN = 0;
    __builtin_enable_interrupts();
    
    while(1) {
        // reseting colors of each LED
        colr[0] = HSBtoRGB(hue1,sat,bright);
        colr[1] = HSBtoRGB(hue2,sat,bright);
        colr[2] = HSBtoRGB(hue3,sat,bright);
        colr[3] = HSBtoRGB(hue4,sat,bright);
        
        ws2812b_setColor(colr, 4);
        
        
        // update the colors
        hue1 = hue1 + update;
        hue2 = hue2 + update;
        hue3 = hue3 + update;
        hue4 = hue4 + update;
        
        // can't let the colors go beyond the scale
        if (hue1 > 360){
            hue1 = 0;
        }
        else if (hue2 > 360){
            hue2 = 0;
        }
        else if (hue3 > 360){
            hue3 = 0;
        }
        else if (hue4 > 360){
            hue4= 0;
        }
    }
}