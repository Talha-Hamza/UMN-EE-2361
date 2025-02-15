.include "xc.inc" 
.text                       
;BP (put the following data in ROM(program memory)) 
; This is a library, thus it can *not* contain a _main function: the C file will 
; define main().  However, we will need a .global statement to make available ASM 
; functions to C code. 
; All functions utilized outside of this file will need to have a leading  
; underscore (_) and be included in a comment delimited list below. 
.global _write_0, _write_1,  _wait_100us, _wait_1ms
    
        
_wait_1ms:
    repeat #15993
    nop

_wait_100us:
    repeat #1593
    nop
    return
    
; 25 Cycles Period
_write_0:	; assuming LOW R0 
		; 2 cycles
    inc LATA	; 1 cycle
    repeat #4	; 1 cycle
    nop		; 4+1 cycles
    
    clr LATA	; 1 cycle
    repeat #9	; 1 cycle to load
    nop		; 9+1 cycles
        
    return	; 3 cycles
    
; 25 Cycles Period    
_write_1:	; assuming LOW R0 
		; 2 cycles
    inc LATA	; 1 cycle
    repeat #10	; 1 cycle
    nop		; 10+1 cycles
    
    clr LATA	; 1 cycle
    repeat #2	; 1 cycle to load
    nop		; 2+1 cycles
        
    return	; 3 cycles

