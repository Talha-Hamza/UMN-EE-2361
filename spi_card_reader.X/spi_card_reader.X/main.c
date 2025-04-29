/*
 * File:   main.c
 * Author: realm
 *
 * Created on April 18, 2025, 3:35 PM
 */

//PRAGMA/Include statements (Ripped from discussion 10 "SPI.C")
#include <p24Fxxxx.h>
#include <stdlib.h>
// PIC24FJ64GA002 Configuration Bit Settings
// CW1: FLASH CONFIGURATION WORD 1 (see PIC24 Family Reference Manual 24.1)
#pragma config ICS = PGx1 // Comm Channel Select (Emulator EMUC1/EMUD1 pins are shared with PGC1/PGD1)
#pragma config FWDTEN = OFF // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config GWRP = OFF // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF // JTAG Port Enable (JTAG port is disabled)
// CW2: FLASH CONFIGURATION WORD 2 (see PIC24 Family Reference Manual 24.1)
#pragma config POSCMOD = NONE // Primary Oscillator Select (Primary oscillator disabled.
// Primary Oscillator refers to an external osc connected to the OSC1 and OSC2 pins)
#pragma config I2C1SEL = PRI // I2C1 Pin Location Select (Use default SCL1/SDA1 pins)
#pragma config IOL1WAY = OFF // IOLOCK Protection (IOLOCK may be changed via unlocking seq)
#pragma config OSCIOFNC = ON // OSC2/CLKO/RC15 functions as port I/O (RC15)
#pragma config FCKSM = CSECME // Clock Switching and Monitor (Clock switching is enabled,
// Fail-Safe Clock Monitor is enabled)
#pragma config FNOSC = FRCPLL // Oscillator Select (Fast RC Oscillator with
#include "xc.h"

void delay_ms(unsigned int ms){
    while (ms-- > 0){
        asm("repeat #15998");
        asm("nop");
    }
}

void setup() {
    CLKDIVbits.RCDIV = 0; //Ensures CLK to be 16MHz
    AD1PCFG = 0x9fff; //Sets ports to digital
    
    //Provide initialization for PIC24 pins and their connections to the RFID device
    
    SPI1STATbits.SPIEN = 0; //Disable SPI1 Module for setup
    
    //Configure Pin out
    __builtin_write_OSCCONL(OSCCON & 0xbf); // unlock PPS
    RPINR20bits.SDI1R = 0b01100; //Set MISO (data in) to RP12 aka Pin 23
    RPOR5bits.RP11R = 0b00111; //Set MOSI (data out) to RP11 aka Pin 
    RPOR7bits.RP14R = 0b01000; //Set SCK (clock output) to RP14 aka Pin 25
    __builtin_write_OSCCONL(OSCCON | 0x40); // lock PPS
    
    
    //Configure the Internal SPI1 device
    SPI1CON1 = 0; //Reset Control Register To Default
    SPI1CON1bits.MSTEN = 1; //Master Mode Enable
    SPI1CON1bits.CKP = 0; //Clock Idle Low
    SPI1CON1bits.CKE = 1; //Data change on falling edge
    SPI1CON1bits.PPRE = 0b01; //Primary Prescalar to 16:1 to ensure 1Mhz operation
    SPI1CON1bits.SPRE = 0b111; //Secondary Prescalar to 1:1 to ensure 1Mhz operation
    SPI1CON2 = 0; //Not dealing with SPI1CON2 so we will just reset it to default
    
    TRISBbits.TRISB12 = 1;  // MISO is input
    TRISBbits.TRISB15 = 0; // Set CS to output
    LATBbits.LATB15 = 1; //Set CS to active low
    
    _SPI1IF = 0; //Reset SPI1 Interrupt Flag to 0
    _SPI1IE = 0; //Enable SPI1 Interrupt Process
    
    
    SPI1STATbits.SPIEN = 1; //Re-Enable SPI Module
}

void write(int address, int val) {
    LATBbits.LATB15 = 0; //activate the RC522 device (active low)         
    SPI1BUF = (address << 1) & 0x7E; //shift address left for padding and mask it for write mode
    while (!SPI1STATbits.SPIRBF); //wait for SPI comm to finish
    SPI1BUF = val; //write value to device via SPI               
    while (!SPI1STATbits.SPIRBF); //wait for SPI comm to finish
    LATBbits.LATB15 = 1; //deactivate the RC522 device (active low)      
}

int read(int address) {
    delay_ms(10);
    SPI1BUF = 0;
    LATBbits.LATB15 = 0; //activate the RC522 device (active low)
    SPI1BUF = (((address << 1) & 0x7E) | 0x80); //shift address left for padding and mask it for read mode
    while (!SPI1STATbits.SPIRBF); //wait for SPI comm to finish
    int temp = SPI1BUF; //dummy read to clear buffer  
    while (!SPI1STATbits.SPIRBF);    
    SPI1BUF = (((address << 1) & 0x7E) | 0x80); //dummy write to get to the next bit             
    while (!SPI1STATbits.SPIRBF); //wait for SPI comm to finish
    int val = SPI1BUF; //retrieve register value from buffer
    LATBbits.LATB15 = 1; //deactivate the RC522 device (active low)         
    return val; //return value read
}

void rc522_init() {
    write(0x01, 0x0F); //write soft reset command to command register
    delay_ms(100);
    
    // 2. Enable Antenna
    write(0x14, 0x63);  // write TX1/TX2 enable command to the TxControl register
    delay_ms(5);
    write(0x26, 0xE0);  // write Gain = 48dB to the RF config register
    
    // 3. Set Timeouts
    write(0x2A, 0x9D);  // Set timeout timer to auto start
    write(0x2B, 0x3E);  // TPrescaler = 2kHz => 1/2kHz = 0.5ms
    write(0x2C, 30);    // 30*0.5ms = 15ms timeout
    
    // 4. Clear FIFO
    write(0x0A, 0x80);  // Flush FIFO
}

int tag_polling() {
    write(0x01, 0x00); //enter idle mode (clear active command)
    write(0x04, 0x00); //clear all interrupt flags
    write(0x09, 0x26);  // load REQA command into FIFO buffer (needed to comm with RFID tags)
    write(0x01, 0x0C);  // execute that command as a Transceive command (activates the antenna)
    
    if (read(0x04) & 0x20) { //check confirmation register bit
        return 1;
    }
    return 0;
}

void test_spi_basic() {
    LATBbits.LATB15 = 0; // CS low
    SPI1BUF = 0xAA; // Send test pattern
    while(!SPI1STATbits.SPIRBF); // Wait for completion
    int received = SPI1BUF;
    LATBbits.LATB15 = 1; // CS high
    
    // Check loopback (short MISO to MOSI for test)
    if(received != 0xAA) {
        while(1); // Hang here if SPI fails (check with debugger)
    }
}

int main(void) {
    int tag_detect = 0; //track if tag has been detected
    setup(); //see setup
    
    //test_spi_basic();
    
    
        // Add this after setup() but before rc522_init():
    // Test SPI by writing/reading a known register
//    write(0x01, 0x00);  // Write to CommandReg
//    if(read(0x01) != 0x00) {  // Should read back 0x00 (idle state)
//        tag_detect = 0;
//    }
//    else {
//        tag_detect = 1;
//    }
//    
//    
    rc522_init(); //see rc522_init
    
    while(1){
        if(tag_polling()) {
             tag_detect = 1;
            delay_ms(500);
        }
        else {
            tag_detect = 0;
        }
        delay_ms(200);
    }
    
    return 0;
}
