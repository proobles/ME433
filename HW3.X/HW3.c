#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "i2c_master_noint.h"
#include<math.h>
#define pi 3.14159265
#define Wadd 0b01000000
#define Radd 0b01000001


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

unsigned char button;

int main(){
    __builtin_disable_interrupts();
    i2c_master_setup();
    __builtin_enable_interrupts();
    TRISAbits.TRISA4 = 0; // A4 is output
    LATAbits.LATA4 = 0;   // A4 default to LOW
    
    // Communicating to MCP
    // Talking to IODIRA, set all A as output
    setPin(Wadd,0x00,0x00);
    // talking to IODIRB, set all B as input
    setPin(Wadd,0x01,0xFF);
    // Talking to OLATA, turn A7 off
    setPin(Wadd,0x14,0x00);
    
    while(1){
        LATAbits.LATA4 = 0;
        _CP0_SET_COUNT(0);
        while (_CP0_GET_COUNT() < 48000000/8) { // .25 sec
        }
        LATAbits.LATA4 = 1;
        _CP0_SET_COUNT(0);
        while (_CP0_GET_COUNT() < 48000000/8) { // .25 sec
        }
        button = readPin(Wadd,0x13,Radd); // listen to GPIOB, note button reading
        if (button == 0x00) {
            setPin(Wadd, 0x14, 0x80); // talk to OTALA, turn on pin A7
        }
        else {
            setPin(Wadd, 0x14, 0x00); // talk to OTALA, turn off pin A7
        }
    }
}