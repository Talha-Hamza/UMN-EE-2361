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

_main:

    bclr CLKDIV,#8  ;BP
    nop
    

    MOV     #0, W2	; i
    MOV     #11, W3	; Number of times you want to run +1 ( to acount for 0)
    ClR	    W4		; Accumulator Register
    MOV	    #2, W5	; fixed multiplier 2
    CLR	    W6
LOOP: 
    
    MUL.SS  W5, W2, W6	; 2*i stored in W6
    ADD	    #1, W6	; 2*i+1
    ADD	    W6, W4, W4	; Accumulate 2*i+6 in W4    
    
    ADD	    #1, W2	; increment i
    
    DEC	    W3, W3	; decrement counter
    BRA	    NZ, LOOP	; keep it running until counter is 0	    


.end