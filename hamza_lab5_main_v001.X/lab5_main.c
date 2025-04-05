#include "xc.h"
#include "stdint.h"
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

char to_print[] = {'W', 'e', 'l', 'c','o','m','e',' ','G','o','p','h','e','r','s','!','\0'};
//char to_print[] = {'1', '2', '3', '4','5','6','7','8','9','A','B','C','D','E','F','J','\0'}; // testing purposes only
                
int main(void) {
    
    setup();
    lcd_init();
    int count = 0;
    
    lcd_setCursor(0, 0);    // always print at the start of the first row
   
    while(1){
        
        lcd_setCursor(0, 0);
        lcd_printStr(to_print, count);
        count += 1;

        if(count == 16){count = 0;}

        delay_ms(1000);}
    
    return 0;
}