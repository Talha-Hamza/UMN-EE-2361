#include "xc.h"
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

//Define Circular Buffer
#define LEN 8
int buf[LEN];
int end = 0;
int start = 0;

//Initalize the Circular Buffer
void initBuffer(void) {
    //Iterates through the array and adds 0s to every slot
    for (int i = 0; i < LEN; i++) {
        putVal(0);
    }
}

//Put a value into the circular buffer
void putVal(int item) {
    buf[end++] = item;
    end %= LEN; // redefines bounds of array 0<=end<=LEN-1
}

//Get the average of the items contained in the circular buffer.
int getAvg(void) {
    int sum = 0;
    int num = 0;
    for (int i = 0; i < LEN; i++) {
        sum += buf[i];
        num++;
    }
    if (sum != 0) {
        sum = (sum / num);
    }
    
    return sum;
}

//Run shit lmao
int main(void) {
    initBuffer();
    int num = getAvg();
    putVal(1);
    putVal(2);
    putVal(3);

    num = getAvg();
    return 0;
}
