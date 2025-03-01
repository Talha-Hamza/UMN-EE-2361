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
    
    MOV	    #11, W1  ; Loop Counter ( from 1 up to power of W1)
    
    CLR	    W6
    CLR     W4      ; Total sum (initialize to 0)

LOOP:
    MOV	    #4, W5
  
    DEC	    W1, W1
    BRA	    NZ, LOOP2
    BRA	    Z, END_LOOP

LOOP2:
    ADD	    W1, W4, W4
    DEC	    W5, W5
    BRA	    NZ, LOOP2
    BRA	    Z, LOOP

END_LOOP:
.end