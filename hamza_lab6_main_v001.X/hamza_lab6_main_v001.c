#include <xc.h>
#include <stdio.h>
#include "lcd_library.h"
#include "circular_buffer.h"
#include "adc_library.h"

int main(void) {
    setup();
    lcd_init();
    initBuffer();
    ADC_init();
    TMR1_init();

    int count = 0;
    
    while(1) {
        count +=1; // waste time as eveything is handled by interrupts
    }
    
    return 0;
}