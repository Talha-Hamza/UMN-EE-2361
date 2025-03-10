#include "xc.h"
#include "stdint.h"
#include <stdio.h>
#include "hamza_lab3_asmb_v001.h"
#include "keypadLib.h"


#define LEFT 1
#define RIGHT 0

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

char pressedKey = 'x';
char lastKey = 'x';
char secondLastKey = 'x';


void setup(void) {
    CLKDIVbits.RCDIV = 0;  // Set RCDIV=1:1 (default 2:1) 32MHz or FCY/2=16MHz
}

int main(void) {
    setup();
    init7seg();
    initKeyPad();
    
while (1) {
    
    pressedKey = readKeypad();
    
    if (pressedKey != 'x' && pressedKey != lastKey) {  
        secondLastKey = lastKey; 
        lastKey = pressedKey;  }

    showChar7seg(secondLastKey, LEFT);
    delay(20);
    showChar7seg(lastKey, RIGHT);}
    

return 0;}

