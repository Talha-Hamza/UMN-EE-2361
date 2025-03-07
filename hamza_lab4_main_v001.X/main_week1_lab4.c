#include "xc.h"


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
    AD1PCFG = 0xffff;
}

void initServo(){
    TRISBbits.TRISB6 = 0; // Setting pin RB6 as output
    
    __builtin_write_OSCCONL(OSCCON & 0xbf); // unlock PPS
//    RPINR7bits.IC1R = 8;  // Use Pin RP8 = "8", for Input Capture 1 (Table 10-2)
    RPOR3bits.RP6R = 18;  // Use Pin RP6 for Output Compare 1 = "18" (Table 10-3)
    __builtin_write_OSCCONL(OSCCON | 0x40); // lock   PPS
    
    OC1CON = 0 ;    // start with a clean slate
    OC1CONbits.OCTSEL = 0b1;    //using timer 3
    OC1CONbits.OCM = 0b110;
    
    T3CON = 0;
    T3CONbits.TCKPS = 0b01; // selecting pre-scaler 8
    TMR3 = 0;
    PR3 = 40000-1;           //20 millisecond with pre-scaler 8
    T3CONbits.TON = 1;       // Turn on timer   
}

void delay_ms(unsigned int ms){
    while (ms-- > 0){
        asm("repeat #15998");
        asm("nop");
    }
}

// this function sets the servo to an angle, depending on the input 
// The range of value will be between 0 and PR3
void setServo(int Val){
    OC1RS = Val;
}

int main(void) {
    
    setup();
    initServo();
    
    while(1){
        setServo(0.25*PR3); 
        delay_ms(1000);
        setServo(0.75*PR3);     // this function make a pulse width a 25% duty cycle
        delay_ms(1000);
    }
    
    return 0;
}
