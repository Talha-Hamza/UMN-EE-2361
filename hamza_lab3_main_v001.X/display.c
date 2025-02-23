#include "xc.h"
#include "stdint.h"
#include <stdio.h>
#include "hamza_lab3_asmb_v001.h"

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

//void delay(int delay_in_ms);

int readKeyPadRAW(void);
char mapKey(int key);
int checkCols(int row);
void initKeyPad(void);


void setup(void) {
    CLKDIVbits.RCDIV = 0;  // Set RCDIV=1:1 (default 2:1) 32MHz or FCY/2=16MHz
}

int main(void) {
    setup();
    init7seg();
    initKeyPad();
    
while (1) {
    // **Activate FIRST Row (RB12 LOW)**
    LATB |= 0x7000;  // Set all rows HIGH first (reset previous state)
    LATB &= 0xEFFF;  // Set RB12 LOW (Activate FIRST row)
    delay(5);

    if (!(PORTA & 0x0008)) { showChar7seg('1', LEFT); }
    else if (!(PORTA & 0x0004)) { showChar7seg('2', LEFT); }
    else if (!(PORTA & 0x0002)) { showChar7seg('3', LEFT); }
    else if (!(PORTA & 0x0001)) { showChar7seg('A', LEFT); }

    delay(5);  // Short delay to avoid ghosting

    LATB |= 0xF000;  // Reset previous row state
    LATB &= 0xDFFF;  // Set RB13 LOW (Activate SECOND row)
    delay(5);

    if (!(PORTA & 0x0008)) { showChar7seg('4', LEFT); }
    else if (!(PORTA & 0x0004)) { showChar7seg('5', LEFT); }
    else if (!(PORTA & 0x0002)) { showChar7seg('6', LEFT);}
    else if (!(PORTA & 0x0001)) { showChar7seg('b', LEFT); }
    
    LATB |= 0xF000;  // Reset previous row state
    LATB &= 0xBFFF;  // Set RB14 LOW (Activate SECOND row)
    delay(5);

    if (!(PORTA & 0x0008)) { showChar7seg('7', LEFT); }
    else if (!(PORTA & 0x0004)) { showChar7seg('8', LEFT); }
    else if (!(PORTA & 0x0002)) { showChar7seg('9', LEFT);}
    else if (!(PORTA & 0x0001)) { showChar7seg('C', LEFT); }

    LATB |= 0xF000;  // Reset all rows first
    LATB &= 0x7FFF;  // Set RB15 LOW (Activate LAST row)
    delay(5);

    if (!(PORTA & 0x0008)) { showChar7seg('E', LEFT); }
    else if (!(PORTA & 0x0004)) { showChar7seg('0', LEFT); }
    else if (!(PORTA & 0x0002)) { showChar7seg('F', LEFT); }
    else if (!(PORTA & 0x0001)) { showChar7seg('d', LEFT); }

    LATB |= 0xF000;  // Reset all rows before next cycle

}
}

    

void initKeyPad(void) {
    AD1PCFG = 0xFFFF;  // Ensure all pins are digital

    // Set RA3-RA0 (Columns) as inputs
    TRISA |= 0x000F;

    // Set RB15-RB12 (Rows) as outputs
    TRISB &= ~0xF000;

    // Enable internal pull-up resistors for RA0-RA3
    CNPU1bits.CN2PUE = 1;  // Enable pull-up on RA0
    CNPU1bits.CN3PUE = 1;  // Enable pull-up on RA1
    CNPU2bits.CN29PUE = 1; // Enable pull-up on RA2
    CNPU2bits.CN30PUE = 1; // Enable pull-up on RA3

    // Initialize row outputs to HIGH
    LATB |= 0xF000;
}

int checkCols(int row) {
    unsigned int mask = 0x0001;  // Start checking from the first column (RA0)
    
    for (int col = 0; col < 4; col++) {
        if (!(PORTA & mask)) {  // Active-low detection (0 means key press)
            return col;  // Return the column index
        }
        mask <<= 1;  // Shift mask to check next column
    }
    return -1;  // No key press detected
}

int readKeyPadRAW(void) {
    unsigned int rowMask = 0x7FFF;  // Initialize row control mask
    int detectedCol;
    static int lastState = -1;  // Stores last detected key state

    for (int row = 0; row < 4; row++) {
        LATB |= 0xF000;  // Set all rows HIGH
        LATB &= rowMask;  // Activate only the current row (LOW)

        detectedCol = checkCols(row);
        
        if (detectedCol != lastState) {  // Edge detection: Only register new key presses
            detectedCol = checkCols(row);  // Confirm by checking again
            if (detectedCol >= 0) {
                LATB &= 0x0FFF;  // Reset rows before returning
                return (row * 4) + detectedCol;  // Unique key ID
            }
        }
        lastState = detectedCol;
        rowMask = (rowMask >> 1) | 0x8000;  // Shift row activation downward
    }
    return -1;  // No key press detected
}

char mapKey(int key) {
    const char keyMap[16] = {
        '1', '2', '3', 'A',
        '4', '5', '6', 'B',
        '7', '8', '9', 'C',
        '*', '0', '#', 'D'
    };

    if (key < 16) {
        return keyMap[key];
    } else {
        return '\0'; // No valid key pressed
    }
}
