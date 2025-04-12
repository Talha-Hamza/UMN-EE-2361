/* 
 * File:   lcd_library.h
 * Author: Hamza
 *
 * Created on March 28, 2025, 12:28 PM
 */

#ifndef LCD_LIBRARY_H
#define	LCD_LIBRARY_H

#define WRITE_ADDRESS 0b01111000     
#define READ_ADDRESS 0b01111001

#define DATA_CONTROL 0b01000000  // 1 for data not command
#define COMMAND_CONTROL 0b00000000

void delay_ms(unsigned int ms){
    while (ms-- > 0){
        asm("repeat #15998");
        asm("nop");
    }
}

void setup(void) {
    CLKDIVbits.RCDIV = 0;  // Set RCDIV=1:1 (default 2:1) 32MHz or FCY/2=16MHz

    TRISBbits.TRISB8 = 1;  // set pin RB8 as input 
    TRISBbits.TRISB5 = 0;  // heartbeat LED as output
    TRISBbits.TRISB6 = 0;  // reset output

    AD1PCFG = 0x9fff;

    I2C1CON = 0;                 // Reset I2C control register
    I2C1CONbits.I2CEN = 0;       // Disable I2C before config
    I2C1CONbits.A10M = 0;        // 7-bit addressing
    I2C1CONbits.DISSLW = 1;      // Disable slew rate for standard speed
    I2C1BRG = 0x9D;              // Baud rate 
    I2C1CONbits.I2CEN = 1;       // Enable I2C

    _MI2C1IF = 0;
    I2C1CONbits.PEN = 1;
    while( I2C1CONbits.PEN );

    delay_ms(20);
    LATBbits.LATB6 = 0;
    delay_ms(20);
    LATBbits.LATB6 = 1;
    delay_ms(20);       
}

void lcd_cmd(char command){
    LATBbits.LATB5 = 1;
    I2C1CONbits.SEN = 1;    
    while(I2C1CONbits.SEN); 
    
    _MI2C1IF = 0;
    I2C1TRN = WRITE_ADDRESS;      
    while(!_MI2C1IF && I2C1STATbits.TRSTAT);

    _MI2C1IF = 0;
    I2C1TRN = COMMAND_CONTROL;
    while(!_MI2C1IF && I2C1STATbits.TRSTAT);
    
    _MI2C1IF = 0;
    I2C1TRN = command;
    while(!_MI2C1IF && I2C1STATbits.TRSTAT);
    
    I2C1CONbits.PEN = 1;
    while(I2C1CONbits.PEN);
    LATBbits.LATB5 = 0;

    delay_ms(1); 
}    

void lcd_printChar(char myChar){
    LATBbits.LATB5 = 1;
    I2C1CONbits.SEN = 1;    
    while(I2C1CONbits.SEN); 
    
    _MI2C1IF = 0;
    I2C1TRN = WRITE_ADDRESS;      
    while(!_MI2C1IF && I2C1STATbits.TRSTAT);

    _MI2C1IF = 0;
    I2C1TRN = DATA_CONTROL;
    while(!_MI2C1IF && I2C1STATbits.TRSTAT);
    
    _MI2C1IF = 0;
    I2C1TRN = myChar;
    while(!_MI2C1IF && I2C1STATbits.TRSTAT);
    
    I2C1CONbits.PEN = 1;
    while(I2C1CONbits.PEN);
    LATBbits.LATB5 = 0;

    delay_ms(1); 
}    

void lcd_init(void){
    lcd_cmd(0x3A);
    lcd_cmd(0x09);
    lcd_cmd(0x06);
    lcd_cmd(0x1E);
    lcd_cmd(0x39);
    lcd_cmd(0x1B);
    lcd_cmd(0x6E);
    lcd_cmd(0x56);
    lcd_cmd(0x7A);
    lcd_cmd(0x38);
    lcd_cmd(0x0F);

    lcd_cmd(0x3A);
    lcd_cmd(0x09);
    lcd_cmd(0x1A);
    lcd_cmd(0x3C);

    lcd_cmd(0x01); // clear display
}

void lcd_setCursor(char x, char y){
    char temp_base = (0x20 * y) + x;
    char base_address = 0x80 | temp_base;
    lcd_cmd(base_address);
}

void lcd_printStr(const char *str) {
    int i = 0;
    while(str[i] != '\0') {
        lcd_printChar(str[i]);
        i++;
    }
}

#endif	/* LCD_LIBRARY_H */
