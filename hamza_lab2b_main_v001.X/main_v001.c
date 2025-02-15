#include "xc.h"
#include "hamza_lab2_asmb_v001.h"
#include "stdint.h"
#include <stdio.h>

// CW1: FLASH CONFIGURATION WORD 1 (see PIC24 Family Reference Manual 24.1)
#pragma config ICS = PGx1          // Comm Channel Select (Emulator EMUC1/EMUD1 pins are shared with PGC1/PGD1)
#pragma config FWDTEN = OFF        // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config GWRP = OFF          // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF           // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF        // JTAG Port Enable (JTAG port is disabled)


// CW2: FLASH CONFIGURATION WORD 2 (see PIC24 Family Reference Manual 24.1)
#pragma config I2C1SEL = PRI       // I2C1 Pin Location Select (Use default SCL1/SDA1 pins)
#pragma config IOL1WAY = OFF       // IOLOCK Protection (IOLOCK may be changed via unlocking seq)
#pragma config OSCIOFNC = ON       // Primary Oscillator I/O Function (CLKO/RC15 functions as I/O pin)
#pragma config FCKSM = CSECME      // Clock Switching and Monitor (Clock switching is enabled, 
                                       // Fail-Safe Clock Monitor is enabled)
#pragma config FNOSC = FRCPLL      // Oscillator Select (Fast RC Oscillator with PLL module (FRCPLL))

uint32_t Wheel(unsigned char WheelPos);
unsigned char getR(uint32_t packedColor);
unsigned char getG(uint32_t packedColor);
unsigned char getB(uint32_t packedColor);
void delay(int delay_in_ms);


#define PERIOD 5 


void setup(void) {
    CLKDIVbits.RCDIV = 0;  // Set RCDIV=1:1 (default 2:1) 32MHz or FCY/2=16MHz
    AD1PCFG = 0x9fff;      // Set all pins to digital mode
    TRISA = 0b1111111111111110; // Set RA0 to output
    LATA = 0x0000;  
//    wait_1ms();
}

int main(void) {
    setup();

    while (1) {
        for (int a = 0; a <= 255; a++) {
            uint32_t color = Wheel(a);  // Get packed color
            int r = getR(color);    // Extract red component
            int g = getG(color);    // Extract green component
            int b = getB(color);    // Extract blue component
            
            writeColor(r, g, b);  // Send to LED
            delay(50);            // Delay for smooth transition
        }
    } 

    return 0;
}


    
    
void writeColor(int r, int g, int b) {
    int i;
    for(i = 7; i >= 0; i--) {
        r & (1 << i) ? write_1() : write_0();
    }
    for(i = 7; i >= 0; i--) {
        g & (1 << i) ? write_1() : write_0();
    }
    for(i = 7; i >= 0; i--) {
        b & (1 << i) ? write_1() : write_0();
    }
    wait_100us();
}

void delay(int delay_in_ms) {
    int i = 0;
    for (i = 0; i <= delay_in_ms; i++){
        wait_1ms();
    }
    
}

uint32_t packColor(unsigned char red, unsigned char green, unsigned char blue) {
    return ((long) red << 16) | ((long) green << 8) | (long) blue;
}

unsigned char getR(uint32_t packedColor) {
    return (unsigned char)( packedColor >> 16);
}

unsigned char getG(uint32_t packedColor) {
    return (unsigned char) (packedColor >> 8);
}

unsigned char getB(uint32_t packedColor) {
    return (unsigned char) packedColor;
}

uint32_t Wheel(unsigned char WheelPos) {
    WheelPos = 255 - WheelPos;
    if (WheelPos < 85) {
        return packColor(255 - WheelPos * 3, 0, WheelPos * 3);
    } else if (WheelPos < 170) {
        WheelPos -= 85;
        return packColor(0, WheelPos * 3, 255 - WheelPos * 3);
    } else {
        WheelPos -= 170;
        return packColor(WheelPos * 3, 255 - WheelPos * 3, 0);
    }
}