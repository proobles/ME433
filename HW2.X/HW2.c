#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "SPI.h"
#include<math.h>
#define pi 3.1416


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
// initialize SPI1
void initSPI();
unsigned char spi_io(unsigned char o);
int main() {

    __builtin_disable_interrupts(); // disable interrupts while initializing things
   
    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // do your TRIS and LAT commands here
    //TRISBbits.TRISB4 = 1; // B4 is input
    //TRISAbits.TRISA4 = 0; // A4 is output
    //LATAbits.LATA4 = 0;   // A4 default to LOW
    
    initSPI();
    __builtin_enable_interrupts();
    unsigned char i = 0;
    unsigned short p,p1;
    unsigned char c,c1;
    c=1;
    c1=0;
    unsigned short v[200];
    unsigned short v1[200];
    int j = 0;
    for (j =0; j<=99; j++){
        v[j] = j*40.96;
    }
    for (j=0; j<=99;j++){
        v[j+100] = (4055.04-(j*40.96));
    }
    for (j=0; j<=199; j++){
        v1[j] = sin((j/99.0)*2*pi)*2048+2048;
    }
    
    while (1) {
        // use _CP0_SET_COUNT(0) and _CP0_GET_COUNT() to test the PIC timing
        // remember the core timer runs at half the sysclk
        LATAbits.LATA0 = 0;
        p = (c<<15);
        p = p|(0b111<<12);
        p = p|(v[i]);
        spi_io(p>>8);
        spi_io(p);
        LATAbits.LATA0 = 1;
        LATAbits.LATA0 = 0;
        p1 = (c1<<15);
        p1 = p1|(0b111<<12);
        p1 = p1|(v1[i]);
        spi_io(p1>>8);
        spi_io(p1);
        LATAbits.LATA0 = 1;
        i++;
        if (i == 200) {
            i = 0;
        }
        _CP0_SET_COUNT(0);
        while (_CP0_GET_COUNT() < 48000000/400) { // .01 sec
        }
    }
}

// initialize SPI1
void initSPI() {
    // Pin B14 has to be SCK1
    
    // Turn off analog pins
    ANSELA= 0;
    // Make an output pin for CS
    TRISAbits.TRISA0 = 0;
    LATAbits.LATA0 = 1;
    //...
    // Set SDO1
    RPB8Rbits.RPB8R = 0b0011;
    // Set SDI1
    SDI1Rbits.SDI1R = 0b0001;

    // setup SPI1
    SPI1CON = 0; // turn off the spi module and reset it
    SPI1BUF; // clear the rx buffer by reading from it
    SPI1BRG = 240; // 1000 for 24kHz, 1 for 12MHz; // baud rate to 10 MHz [SPI1BRG = (48000000/(2*desired))-1]
    SPI1STATbits.SPIROV = 0; // clear the overflow bit
    SPI1CONbits.CKE = 1; // data changes when clock goes from hi to lo (since CKP is 0)
    SPI1CONbits.MSTEN = 1; // master operation
    SPI1CONbits.ON = 1; // turn on spi 
}

unsigned char spi_io(unsigned char o) {
    SPI1BUF = o;
    while (!SPI1STATbits.SPIRBF) {
        ;
    }
    return SPI1BUF;
}