#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<stdio.h>
#include<string.h>
#include "i2c_master_noint.h"
#include "font.h"
#include "ssd1306.h"
#define Wadd 0b01000000
#define Radd 0b01000001

char mes[30],frame[40];
int lol = 0;
unsigned char t0,t1;
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
#pragma config USERID = 00000000 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations

int main(){
    __builtin_disable_interrupts();
    i2c_master_setup();
    ssd1306_setup();
    __builtin_enable_interrupts();
    TRISAbits.TRISA4 = 0; // A4 is output
    LATAbits.LATA4 = 0;   // A4 default to LOW
    // Communicating to MCP
    // Talking to IODIRA, set all A as output
    setPin(Wadd,0x00,0x00);
    // Talking to OLATA, turn A7 off
    setPin(Wadd,0x14,0x00);
       
        
    while(1){
        _CP0_SET_COUNT(0);
        t0 = _CP0_GET_COUNT();
        while (_CP0_GET_COUNT() < 48000000/8) { // .25 sec
            LATAbits.LATA4 = 1; // Set PIC A4 high
            setPin(Wadd, 0x14, 0x80); // talk to OTALA, set MCP pin A7 high
        }
        while (_CP0_GET_COUNT() < 48000000/4) { // .25 sec
            LATAbits.LATA4 = 0; // Set PIC A4 low
            setPin(Wadd, 0x14, 0x00); // talk to OTALA, set pin A7 low
        }
        
        sprintf(mes,"my var = %d",lol);
        ssd_speak(1,1,&mes);
        ssd_speak(1,20,&frame);
        ssd1306_update();
        t1 = _CP0_GET_COUNT();
        sprintf(frame,"fps = %d",24000000/(t1-t0));
        lol++;
    }
}