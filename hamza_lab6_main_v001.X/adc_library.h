/* 
 * File:   adc_library.h
 * Author: Hamza
 *
 * Created on April 11, 2025, 10:43 PM
 */

#ifndef ADC_LIBRARY_H
#define	ADC_LIBRARY_H

void _ISR _ADC1Interrupt(void) {
    _AD1IF = 0;
    putVal(ADC1BUF0);
}

void ADC_init(void) {

    AD1PCFG = 0xFFFE;       // straight from the datasheet
    
    AD1CON1 = 0;            // reset everything before setup
    AD1CON1bits.ASAM  = 1;  // Sample Auto-Start Mode bit ON
    AD1CON1bits.SSRC = 0b010; // selecting Timer3 compare match
        
    
    AD1CON2 = 0;            // using at completion of each sequence
    
    AD1CON3 = 0;            //reset everything before setup
    AD1CON3bits.ADCS0 = 1;  // A/D Conversion Clock Period set at 2*Tcy
    AD1CON3bits.SAMC = 0x1; // sample time at 1 Tad
    
    AD1CSSL = 0;            // no scanning needed
    AD1CHS = 0;             // no input channels to be amplified

    
    // Using timer 3 to sample 16 times a second (16Hz)
    T3CON = 0;
    T3CONbits.TCKPS = 0b10;     // pre-scaler = 64
    PR3 = 15623;    // Set for 16Hz with system clock at 16MHz 
    //(1953)
    TMR3 = 0;                  
    T3CONbits.TON = 1;
    
    IFS0bits.AD1IF = 0;     // clear A/D conversion interrupt
    IEC0bits.AD1IE = 1;     // enable A/D conversion interrupt
    AD1CON1bits.ADON = 1;   // turn on A/D
}

void _ISR _T1Interrupt(void) {
    _T1IF = 0;
    char to_prinr[9];
    sprintf(to_prinr, "%6.4f V", (3.3 / 1024) * getAvg());
    lcd_cmd(0x01);          // clear old value first
    lcd_printStr(to_prinr);
}

void TMR1_init(void) {
    T1CON = 0;
    PR1 = 24999;        // for a 100ms period   
    T1CONbits.TCKPS = 0b10;        
    TMR1 = 0;
    IEC0bits.T1IE = 1;
    IFS0bits.T1IF = 0;
    T1CONbits.TON = 1;
}
#endif	/* ADC_LIBRARY_H */

