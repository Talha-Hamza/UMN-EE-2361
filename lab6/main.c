#include "xc.h"
#include <stdio.h>
#include "lcd_library.h"


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
#define LEN 128
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


void __attribute__ ((__interrupt__)) _ADC1C1Interrupt(void) {
    _AD1IF = 0;
    int addValue = ADC1BUF0;
    putVal(addValue);
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

void initADC(void) {
    CLKDIVbits.RCDIV = 0;
    AD1PCFG = 0x9fff;
    AD1CON1 = 0;
    AD1CON2 = 0;
    AD1CON3 = 0;
    
    AD1CSSL = 0;
    AD1PCFGbits.PCFG0 = 0; //no clue what this is
    AD1PCFG = 0xFFFE;
    TRISA |= 1;
    IFS0bits.AD1IF = 0;
    IEC0bits.AD1IE = 1;
    
    //Timer3 Config
    T3CON = 0;
    _T3IF = 0;
    T3CONbits.TCKPS = 0b10;
    PR3 = 15625;
    T3CONbits.TON = 1;
    
    //ADC Config

    AD1CON1bits.ASAM = 1; //enable sample auto start
    AD1CON1bits.SSRC = 0b010; //couples timer 3 with the ADC
    
    AD1CON2bits.VCFG = 0b00; //Ground enable
    AD1CON2bits.SMPI = 0b1111; //16 sets of samples per interrupt
    
    AD1CON3bits.ADCS = 0b00001111; //K = 16
    AD1CON3bits.SAMC = 0b10110; //SAMC of 22
    AD1CON1bits.ADON = 1; //enable ADC1
    AD1CON1bits.SAMP = 1; //enable sampling
    delay_ms(10);
    AD1CON1bits.SAMP = 0;
            

            
    
}

//Run shit lmao
int main(void) {
//    initBuffer();
//    int num = getAvg();
//    putVal(1);
//    putVal(2);
//    putVal(3);
//
//    num = getAvg();
    initBuffer();
    initADC();
    setup();
    lcd_init();
    unsigned long int data;
    char dataStr[20];
    while(1) {
        lcd_setCursor(0,0);
        data = getAvg();
        sprintf(dataStr, "%6.4f V", (3.3*data)/1023);
        lcd_printStr(dataStr, 0);
    }
    
    
    return 0;
}
