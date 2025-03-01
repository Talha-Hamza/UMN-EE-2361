.include "xc.inc"

config __CONFIG2, POSCMOD_EC & I2C1SEL_SEC & IOL1WAY_OFF & OSCIOFNC_ON & FCKSM_CSECME & FNOSC_FRCPLL & SOSCSEL_LPSOSC & WUTSEL_FST & IESO_OFF
config __CONFIG1, WDTPS_PS1 & FWPSA_PR32 & WINDIS_OFF & FWDTEN_OFF & BKBUG_ON & GWRP_ON & GCP_ON & JTAGEN_OFF

.bss
counter:
    .space 2  
stack:
    .space  32  

.text           
.global _main  


_main:
    bclr CLKDIV,#8  
    nop
    
    MOV	    #10, W1  ; Loop Counter ( from 1 up to power of W1)
    ADD     W1, #1, W2  
    MOV     #2, W3  ; Base Value (2)
    MOV     #1, W6  ; 2^i Initial Value (2^0 = 1)
    CLR     W4      ; Total sum (initialize to 0)

LOOP:
    ADD     W6, W4, W4   ; total += 2^i
    DEC     W2, W2       ; Decrement main loop counter
    CP0	    W2            ; Check if W2 == 0
    BRA     Z, END_LOOP  ; If W2 == 0, exit

    ; Compute next power of 2
    MUL.SS  W3, W6, W6       ; W6 = W6 * 2  (2^i * 2 = 2^(i+1))
   
    BRA     LOOP         ; Continue looping

END_LOOP:
    DEC     W4, W4

.end