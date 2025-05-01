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
    
    //Provide Pinout Information
    SPI1STATbits.SPIEN = 0; //Disable SPI1 Module for setup
    TRISBbits.TRISB12 = 1;  // MISO is input
    TRISBbits.TRISB15 = 0; // Set CS to output
    TRISBbits.TRISB14 = 0; //Set CLK pin to output
    TRISBbits.TRISB11 = 0; //Set MOSI pin to output
    TRISBbits.TRISB13 = 0; //This is output for a test signal
    
    //Configure Internal Devices
    __builtin_write_OSCCONL(OSCCON & 0xbf); // unlock PPS
    RPINR20bits.SDI1R = 0b01100; //Set MISO (data in) to RP12 aka Pin 23
    RPOR5bits.RP11R = 0b00111; //Set MOSI (data out) to RP11 aka Pin 
    RPOR7bits.RP14R = 0b01000; //Set SCK (clock output) to RP14 aka Pin 25
    __builtin_write_OSCCONL(OSCCON | 0x40); // lock PPS
    
    
    //This could be dangerous -- Delete if things are breaking
//    SPI1STATbits.SPITBF = 1;
//    SPI1STATbits.SPIRBF = 1;
    
    
    //This could also be dangerous -- Delete if things are breaking
//    SPI1CON1bits.DISSCK = 0;
//    SPI1CON1bits.DISSDO = 0;
    
    //Configure the Internal SPI1 device
    SPI1CON1 = 0; //Reset Control Register To Default
    SPI1CON1bits.MSTEN = 1; //Master Mode Enable
    //SPI1CON1bits.MODE16 = 0; //Transmit over 8-bit mode
    SPI1CON1bits.CKP = 0; //Clock Idle Low
    SPI1CON1bits.CKE = 1; //Data change on falling edge
    SPI1CON1bits.SMP = 0; //Sample Input Data at Middle of Output Time
    SPI1CON1bits.PPRE = 0b10; //Primary Prescalar to 4:1 to ensure 4Mhz operation
    SPI1CON1bits.SPRE = 0b111; //Secondary Prescalar to 1:1 to ensure 4Mhz operation
    SPI1CON1bits.SSEN = 0; //Slave select is not used by the module -- Pin is controlled by port function
    SPI1STATbits.SPIROV = 0;
    SPI1CON2 = 0; //Not dealing with SPI1CON2 so we will just reset it to default
    
    
    
    LATBbits.LATB15 = 1; //Set CS to active low
    
    _SPI1IF = 0; //Reset SPI1 Interrupt Flag to 0
    _SPI1IE = 0; //Enable SPI1 Interrupt Process
    
    
    SPI1STATbits.SPIEN = 1; //Re-Enable SPI Module
}

unsigned char encoder_SPI1_transfer(unsigned char data) {
    SPI1BUF = data;
    while(!SPI1STATbits.SPIRBF);
    //SPI1STATbits.SPIRBF = 0;
    return SPI1BUF;
}

void write(unsigned char address, unsigned char data) {

    LATBbits.LATB15 = 0;
    encoder_SPI1_transfer((address << 1) & 0x7E);
    encoder_SPI1_transfer(data);
    LATBbits.LATB15 = 1;
}

unsigned char read(unsigned char address) {
    LATBbits.LATB15 = 0;
    SPI1BUF = (((address << 1) & 0x7E) | 0x80);
    while(!SPI1STATbits.SPIRBF);
    unsigned char temp = SPI1BUF;
    SPI1BUF = 0x00;
    while(!SPI1STATbits.SPIRBF);
    LATBbits.LATB15 = 1;
    return SPI1BUF;
}



//unsigned char read(int address) {
//    unsigned char tracker_read; 
//    LATBbits.LATB15 = 0;
//    encoder_SPI1_transfer(((address << 1) & 0x07E) | 0x80);
//    tracker_read = encoder_SPI1_transfer(0x00); //Dummy Read: Might not be necessary.
//    LATBbits.LATB15 = 1;
//    return tracker_read;
//}

//unsigned char MFRC_readRegister(unsigned char address) {
//    _LATB15 = 0; // CS low
//    
//    // Send address with MSB=1 for read
//    SPI1BUF = ((address & 0x3F) << 1) | 0x80;
//    while(!SPI1STATbits.SPIRBF); // Wait for completion
//    unsigned char dummy = SPI1BUF; // Clear buffer
//    
//    // Send dummy byte to receive data
//    SPI1BUF = 0x00;
//    while(!SPI1STATbits.SPIRBF);
//    unsigned char value = SPI1BUF;
//    
//    _LATB15 = 1; // CS high
//    
//    return value;
//}

void rc522_init() {
    write(0x01, 0x0F); //write soft reset command to command register
    //delay_ms(100);
    
    // 2. Enable Antenna
    write(0x14, 0x33);  // write TX1/TX2 enable command to the TxControl register
    //delay_ms(5);
    write(0x26, 0x70);  // write Gain = 48dB to the RF config register
    
    // 3. Set Timeouts
    write(0x2A, 0x9D);  // Set timeout timer to auto start
    write(0x2B, 0x3E);  // TPrescaler = 2kHz => 1/2kHz = 0.5ms
    write(0x2C, 30);    // 30*0.5ms = 15ms timeout
    
    // 4. Clear FIFO
    write(0x0A, 0x80);  // Flush FIFO
    ;
}

int tag_polling() {
    write(0x01, 0x00); //enter idle mode (clear active command)
    write(0x04, 0x00); //clear all interrupt flags
    write(0x09, 0x26);  // Request command to read RFID (0x26 for REQA, 0x52 for Wake)
    write(0x01, 0x0C);  // execute that command as a Transceive command (activates the antenna)
//    
    if (read(0x04) & 0x20) { //check confirmation register bit
        return 1;
    }
//    return 0;
    return 0;
}

volatile unsigned char tag_detect = 0; //track if tag has been detected
volatile unsigned char irq1 = 0; 
volatile unsigned char irq2 = 0; 

int main(void) {
    setup();
    rc522_init();
//    write(0x04, 0x07);

    while(1) {
        write(0x09, 0x43);
        irq1 = read(0x04); //returns 0x07 ideally
        irq2 = read(0x04); //returns 0x07 ideally
//        delay_ms(100);
    }

    
//    while(1) {
//        // Check ComIrqReg (0x04) for interrupts
//        irq1 = MFRC_readRegister(0x04);
//        
//        // If you want to detect tags, use proper polling:
//        write(0x01, 0x00); // Idle mode
//        write(0x02, 0x00); // Clear IRq
//        write(0x09, 0x26); // Send REQA
//        write(0x01, 0x0C); // Transceive
//        
////        delay_ms(100);
//        
//        // Check if we got a response
//        irq1 = MFRC_readRegister(0x09);
//        if(irq1 & 0x20) { // RxIRq bit
//            tag_detect = 1;
//        } else {
//            tag_detect = 0;
//        }
//        
////        delay_ms(200);
//    }
    return 0;
}