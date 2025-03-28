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


volatile unsigned int dutyCycle = 0;  // positive or negative
volatile unsigned long int LastEventTime = 0;
volatile unsigned int PrevState = 0;

volatile unsigned int rollover2 = 0;
volatile int rollover3 = 0;
volatile long int difference = 0;

int currState = 0;
int prevState = 0;


//Defining Circular Buffer
#define LEN 3
volatile unsigned long int buf[LEN] = {0};
volatile unsigned long int end = 0;
volatile unsigned long int start = 0;

void put(int item) {
    buf[end++] = item;
    end %= LEN;
}
int get(void) {
    int item = buf[start++];
    start %= LEN;
    return item;
}

//IC1 ISR
void __attribute__((__interrupt__,__auto_psv__)) _IC1Interrupt(void)
{
    unsigned long int current; //Current Event Time
    _IC1IF = 0; //Set flag to zero
    
    current = IC1BUF;
    
    
     dutyCycle = ((long)((PR2+1)*(rollover2)) + current) - LastEventTime;
       
    if (dutyCycle <= 140) { //If delay is less than 2ms, then a press happened
        return;
        
    } else {
        LastEventTime = ((PR2+1)*(rollover2)) + current; //Update LastEventTime
        
        if (PrevState == 0) { 
        PrevState = 1;
        put(LastEventTime); 
        rollover2 = 0;
    } else {
        PrevState = 0;
        return;
        }
        
    }
}


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
    _T3IF = 0;             //clear interrupt flag
    _T3IE = 1; 
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

void initPushButton(void){
    
    __builtin_write_OSCCONL(OSCCON & 0xbf); // unlock PPS
    RPINR7bits.IC1R = 8;  // Use Pin RP8 = "8", for Input Capture 1 (Table 10-2)
//    RPOR3bits.RP6R = 18;  // Use Pin RP6 for Output Compare 1 = "18" (Table 10-3)
    __builtin_write_OSCCONL(OSCCON | 0x40); // lock   PPS
    
    TRISBbits.TRISB8 = 1;   // Setting RB8 as input
    CNPU2bits.CN22PUE = 1;  // turn on pull-up for pin 17
    
    
    T2CON = 0;
    T2CONbits.TCKPS = 0b11; // pre-scaler of 256
    PR2 = 62499;
    _T2IF = 0;             //clear interrupt flag
    _T2IE = 1;             //enable interrupt 
    T2CONbits.TON = 1;
    
//    IC1CON = 1; // every edge
    _IC1IF = 0;
    _IC1IE = 1;
    IC1CONbits.ICTMR = 1;
    IC1CONbits.ICM = 0b001;  // every edge
}

void __attribute__((interrupt, auto_psv)) _T2Interrupt(){
     rollover2++;
    _T2IF = 0;
}

void __attribute__((interrupt, auto_psv)) _T3Interrupt(){
     rollover3++;
    _T3IF = 0;
}


unsigned long int start_buf = 0;
unsigned long int mid_buf = 0;
unsigned long int end_buf = 0;
unsigned long int currentTime = 0;

int main(void) {
    setup();
    initServo();
    initPushButton();
    
//    int count; 
    double my_set = 0.025; // START with servo moved RIGHT all the way 
    
    while(1) {
        // Fix: Use the global variable instead of shadowing
        currentTime = rollover2 * 62500 + TMR2;

        if (end != start) { // Check if buffer is not empty
            start_buf = get();
            mid_buf = get();
            end_buf = get();
            
            difference = end_buf - start_buf;
                     
            if (difference >= 15625) {
                my_set = 0.125;   
                difference = 0;      
            }

        }
        
            if ((currentTime - end_buf) >= (3 * 62500)) {
            my_set = 0.025;
            
            for (int i = 0; i < LEN; i++) {
                buf[i] = 0;
            }

            start = 0;
            end = 0;
            }
        
            setServo(my_set * PR3);  
            delay_ms(20);
            }
    
    return 0;
}