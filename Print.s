; Print.s
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 
cnt 			 EQU   -12
sum              EQU   -4
sum2			 EQU   -8
n				 EQU   -16
	
    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix

    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB

  

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutDec
	  PUSH {R11, LR}   ;save R11
	  MOV R11, SP
	  PUSH {R0, R1, R2, R3} ; save number
	  ; Done with allocation
	  MOV  R3, #0
	  STR  R3,[R11,#sum] ; sum = 0
	  STR  R3,[R11,#cnt] ; cnt = 0
outdec_loop
	  LDR R0, [R11,#n]   ; load R3 with number
	  MOV R1, #10
	  UDIV R2, R0, R1      ; modulo
	  MLS  R0, R1, R2, R0
	  ;
	  LDR R3, [R11,#sum]
	  LDR R2, [R11,#cnt]	; set r2 to cnt offset
	  CMP R2, #0x20
	  BLT no_more_offset
	  LDR R3, [R11,#sum2]
	  SUB R2, R2, #0x20
	  LSL R0, R2
	  ADD R3, R0
	  ADD R2, R2, #0x24
	  STR R3, [R11,#sum2]
	  STR R2, [R11,#cnt]
	  B skip
no_more_offset
	  LSL R0, R2
	  ADD R3, R0
	  ADD R2, R2, #0x04		; set next cnt offset
	  STR R3, [R11,#sum]
	  STR R2, [R11,#cnt]
	  ;
skip
	  LDR R3, [R11,#n]   ; load R3 with number
	  UDIV R3, R3, R1
	  STR  R3,[R11,#n]	 ; store r3/10 to the number
	  CMP R3, #0
	  BNE outdec_loop
	  LDR R0, [R11,#sum2]
	  SUB R0, R0, #4
	  STR R0, [R11,#sum2]
outchar_loop
	  LDR R0, [R11,#sum]
	  MOV R2, #0x0F
	  LDR R1, [R11,#cnt]
	  SUB R1, R1, #0x04	
	  STR R1, [R11,#cnt]
	  CMP R1, #0x20
	  BLT no_count_offset
	  LDR R0, [R11,#sum2]
	  SUB R1, R1, #0x20
	  LSL R2, R1
	  AND R0, R0, R2
	  LSR R0, R1
	  ADD R1, R1, #0x20
	  ADD R0, R0, #0x30
	  B outchar
no_count_offset	  
	  LSL R2, R1
	  AND R0, R0, R2
	  LSR R0, R1
	  ADD R0, R0, #0x30
outchar
	  PUSH {R0, R1}
	  BL ST7735_OutChar	
	  POP {R0, R1}
	  CMP R1, #0
	  BGT outchar_loop
	  ; Deallocate now
	  ADD SP, #16
	  POP {R11, LR}
      BX  LR
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000 "
;       R0=3,    then output "0.003 "
;       R0=89,   then output "0.089 "
;       R0=123,  then output "0.123 "
;       R0=9999, then output "9.999 "
;       R0>9999, then output "*.*** "
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutFix
	  PUSH {R11, LR}   ;save R11
	  MOV R11, SP
	  PUSH {R0, R1, R2, R3} ; save number
	  ; Done with allocation
	  MOV  R3, #0
	  STR  R3,[R11,#sum] ; sum = 0
	  STR  R3,[R11,#cnt] ; cnt = 0
outfix_dec_loop
	  LDR R0, [R11,#n]   ; load R3 with number
	  MOV R1, #10
	  UDIV R2, R0, R1      ; modulo
	  MLS  R0, R1, R2, R0
	  ;
	  LDR R3, [R11,#sum]
	  LDR R2, [R11,#cnt]	; set r2 to cnt offset
	  CMP R2, #0x20
	  BLT no_more_offset_fix
	  LDR R3, [R11,#sum2]
	  SUB R2, R2, #0x20
	  LSL R0, R2
	  ADD R3, R0
	  ADD R2, R2, #0x24
	  STR R3, [R11,#sum2]
	  STR R2, [R11,#cnt]
	  B skip_fix
no_more_offset_fix
	  LSL R0, R2
	  ADD R3, R0
	  ADD R2, R2, #0x04		; set next cnt offset
	  STR R3, [R11,#sum]
	  STR R2, [R11,#cnt]
	  ;
skip_fix
	  LDR R3, [R11,#n]   ; load R3 with number
	  UDIV R3, R3, R1
	  STR  R3,[R11,#n]	 ; store r3/10 to the number
	  CMP R3, #0
	  BNE outfix_dec_loop
	  LDR R0, [R11,#sum2]
	  SUB R0, R0, #4
	  STR R0, [R11,#sum2]
outfix_char_loop
	  LDR R1, [R11,#cnt]
	  CMP R1, #0x10
	  BGT overflow
	  MOV R1, #0x14
outfix_loop
	  LDR R0, [R11,#sum]
	  MOV R2, #0x0F
	  SUB R1, R1, #0x04
	  CMP R1, #0x0C
	  BEQ dot_ascii
	  BGT ones_digit
	  STR R1, [R11,#cnt]
	  LSL R2, R1
	  AND R0, R0, R2
	  LSR R0, R1
	  ADD R0, R0, #0x30
outfix_char
	  PUSH {R0, R1}
	  BL ST7735_OutChar	
	  POP {R0, R1}
	  CMP R1, #0
	  BGT outfix_loop
	  ; Deallocate now
	  ADD SP, #16
	  POP {R11, LR}
      BX   LR
dot_ascii
	  MOV R0, #0x2E
	  B outfix_char
ones_digit
	  STR R1, [R11,#cnt]
	  SUB R1, R1, #0x04
	  LSL R2, R1
	  AND R0, R0, R2
	  LSR R0, R1
	  ADD R0, R0, #0x30
	  ADD R1, R1, #0x04
	  B outfix_char
overflow
	  MOV R0, #5
	  STR R0, [R11,#cnt]
overflow_loop
	  LDR R1, [R11,#cnt]
	  CMP R1, #4
	  BEQ dot
	  MOV R0, #0x2A
	  B skip_dot
dot	  MOV R0, #0x2E
skip_dot
	  PUSH {R0, R1}
	  BL ST7735_OutChar	
	  POP {R0, R1}
	  SUB R1, R1, #1
	  STR R1, [R11,#cnt]
	  CMP R1, #0
	  BGT overflow_loop
	  ; Deallocate now
	  ADD SP, #16
	  POP {R11, LR}
      BX   LR
     ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file
