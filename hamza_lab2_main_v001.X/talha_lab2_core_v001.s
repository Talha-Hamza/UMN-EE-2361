.include "xc.inc"          ; required "boiler-plate" (BP)

;the next two lines set up the actual chip for operation - required
config __CONFIG2, POSCMOD_EC & I2C1SEL_SEC & IOL1WAY_OFF & OSCIOFNC_ON & FCKSM_CSECME & FNOSC_FRCPLL & SOSCSEL_LPSOSC & WUTSEL_FST & IESO_OFF
config __CONFIG1, WDTPS_PS1 & FWPSA_PR32 & WINDIS_OFF & FWDTEN_OFF & BKBUG_ON & GWRP_ON & GCP_ON & JTAGEN_OFF

    .bss        ; put the following labels in RAM
counter:
    .space 2    ; a variable that takes two bytes (we won?t use
                ; it for now, but put here to make this a generic
                ; template to be used later).
stack:
    .space  32  ; this will be our stack area, needed for func calls

.text           ; BP (put the following data in ROM(program memory))

;because we are using the C compiler to assemble our code, we need a "_main" label
;somewhere. (There's a link step that looks for it.)
.global _main               ;BP
;your functions go here
    
delay_1_milli:
    repeat #15993
    nop
    return

high24low32:	; 22 cycles in all
		; 2 cycles for function load
    repeat #16	; 1 cycle
    nop		; 16+1 cycles
    
    return	; 3 cycles 

; 25 Cycles Period
write_0:	; assuming LOW R0 
		; 2 cycles
    inc LATA	; 1 cycle
    repeat #4	; 1 cycle
    nop		; 4+1 cycles
    
    clr LATA	; 1 cycle
    repeat #9	; 1 cycle to load
    nop		; 9+1 cycles
        
    return	; 3 cycles
    
; 25 Cycles Period    
write_1:	; assuming LOW R0 
		; 2 cycles
    inc LATA	; 1 cycle
    repeat #10	; 1 cycle
    nop		; 10+1 cycles
    
    clr LATA	; 1 cycle
    repeat #2	; 1 cycle to load
    nop		; 2+1 cycles
        
    return	; 3 cycles


_main:

    bclr    CLKDIV,#8                 ;BP
    nop
    ;; --- Begin your program below here ---
    
    mov #0x9fff,w0  
    mov w0,AD1PCFG
    
    mov #0b1111111111111110,w0 
    mov w0,TRISA
    
    mov #0x0000,w0
    mov w0,LATA	    ; Sets pin RA high
    
    foreverLoop:
    call write_1
    call write_1
    call write_1
    call write_1
    
    call write_1
    call write_1
    call write_1
    call write_1

    call write_1
    call write_0
    call write_0
    call write_0
    
    call write_0
    call write_0
    call write_0
    call write_0

    call write_0
    call write_0
    call write_0
    call write_0
    
    call write_0
    call write_0
    call write_0
    call write_0
    
    call delay_1_milli
    
    bra foreverLoop
    .end
    