/* 
 * File:   lcd_library.h
 * Author: Hamza
 *
 * Created on March 28, 2025, 12:28 PM
 */

#ifndef LCD_LIBRARY_H


#define WRITE_ADDRESS 0b01111000;     
#define READ_ADDRESS 0b01111001;

#define DATA_CONTROL 0b01000000;   // 1 for data not command
#define COMMAND_CONTROL 0b00000000;


void lcd_cmd(char command){

    LATBbits.LATB5 = 1;     // LED low
    I2C1CONbits.SEN = 1;    // start bit 
    while(I2C1CONbits.SEN); // wait for start condition to clear
    
    _MI2C1IF = 0;           // clear flag 
    I2C1TRN = WRITE_ADDRESS;      //send address
    while(!_MI2C1IF && I2C1STATbits.TRSTAT);

    _MI2C1IF = 0;
    I2C1TRN = COMMAND_CONTROL; //send control bit
    while(!_MI2C1IF && I2C1STATbits.TRSTAT);
    
    _MI2C1IF = 0;
    I2C1TRN = command;         //send command
    while(!_MI2C1IF && I2C1STATbits.TRSTAT);
    
    I2C1CONbits.PEN = 1;       //stop bit
    while(I2C1CONbits.PEN);
    LATBbits.LATB5 = 0;
    
    delay_ms(20);

}    

void lcd_printChar(char myChar){

    LATBbits.LATB5 = 1;     // LED low
    I2C1CONbits.SEN = 1;    // start bit 
    while(I2C1CONbits.SEN); // wait for start condition to clear
    
    _MI2C1IF = 0;           // clear flag 
    I2C1TRN = WRITE_ADDRESS;      //send address
    while(!_MI2C1IF && I2C1STATbits.TRSTAT);

    _MI2C1IF = 0;
    I2C1TRN = DATA_CONTROL; //send control bit
    while(!_MI2C1IF && I2C1STATbits.TRSTAT);
    
    _MI2C1IF = 0;
    I2C1TRN = myChar;         //send command
    while(!_MI2C1IF && I2C1STATbits.TRSTAT);
    
    I2C1CONbits.PEN = 1;       //stop bit
    while(I2C1CONbits.PEN);
    LATBbits.LATB5 = 0;
    
    delay_ms(20);

}    


void lcd_init(void){

    lcd_cmd(0x3A); // from initialization table on page 9
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
    
    lcd_cmd(0x3A); // configure 2 line display, taken from page 11
    lcd_cmd(0x09);
    lcd_cmd(0x1A);
    lcd_cmd(0x3C);
    
    lcd_cmd(0x01); // Clean led display, taken from page 41 SSD1803 

}

void lcd_setCursor(char x, char y){
    char temp_base = (0x20 * y) + x; // formula from the lab manual 
    char base_address = 0x80  | temp_base ;  // ensuring MSB is 1
    lcd_cmd(base_address);
    
}


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
    I2C1CONbits.I2CEN = 0;  // disable before setting baud rate
    I2C1BRG = 0x9D;
    I2C1CONbits.I2CEN = 1;  // enable after setting baud rate
    _MI2C1IF = 0;
    
    I2C1CONbits.PEN = 1;
    while( I2C1CONbits.PEN );
    
    delay_ms(20);
    LATBbits.LATB6 = 0;     // set LED reset = 0
    delay_ms(20);
    LATBbits.LATB6 = 1;     // set LED reset = 1
    delay_ms(20);       
}

void lcd_printStr(const char *str, int start_pos) {
    
    int str_length = 0; // finding size of string 
    while (str[str_length] != '\0') {
        str_length++;
    } 
    
    for (int i = 0; i < 11; i++) { // printing the length of the available space

        int pos = start_pos + i;
        if (pos >= str_length) {    // means the word has ended and we need to begin
            pos = pos - str_length;  // wrapping around to the start of the string
        }
        
        lcd_printChar(str[pos]);
    }
    
}

#define	LCD_LIBRARY_H



#endif	/* LCD_LIBRARY_H */

