;*******************************************************************************
;Phan Le Son (RBVH/EMB1)
;*******************************************************************************
__CONFIG _CONFIG1, _FOSC_INTOSC & _WDTE_OFF & _MCLRE_OFF & _CP_OFF &  _CLKOUTEN_OFF
__CONFIG _CONFIG2, _PLLEN_ON & _LVP_OFF

#include <p16f1788.inc>
;-------------------------------------------------------------------------------
cblock	0x020
    ;Temp
    ;ADC_AN1_L
    ;ADC_AN1_H
    ;TicCnt
    ;OutH
    ;OutL
    ;Op1
    ;Op2
    ;TempL
    ;TempH
endc

cblock 0x070
    W_Save
    STATUS_Save
    Flags
        Temp
    ADC_AN1_L
    ADC_AN1_H
    TicCnt
    OutH
    OutL
    Op1
    Op2
    TempL
    TempH
    Delay_100
endc
;----------------------------------------------------------------------------------
Tload		equ 0xCD  ; 205
TicLoad     equ 0x7D  ; 64 --> Time = 100*50*16*1/8 us = 10000us = 10ms
Outport		equ	PORTC

;---Flagdef------------------------------------------------------------------------
TicFlag		equ	0
SecFlag     equ 1

;----------Macro----------------------------------------------------------------
Mul	macro Num1,Num2
local multiply_numbers, mult_loop
multiply_numbers
; Takes numbers in Num1 and Num2, and returns product in OutH:OutL
    clrf    OutH            ; clear all non-input variables
    clrf    OutL
mult_loop
    BANKSEL STATUS
    bcf     STATUS,0       ; clear carry bit
    movf    Num2,w            ; Num2-->WREG
    addwf   OutL,1          ; add Num2 to OutL
    btfsc   STATUS,0        ; check carry bit
    incf    OutH,1          ; if set, increment OutH
    decfsz  Num1,f           ; decrement Num1
    goto    mult_loop       ; if Num1 is not zero, repeat loop

endm
;-------------------------------------------------------------------------------
org 0x00
goto Start
;-------------------------------------------------------------------------------------------
;Interrupt is implemented here
nop
nop
nop
ISR
    movwf	W_Save	;Interrupt Vector 04h
	movf	STATUS,w
	movwf	STATUS_Save
	btfsc	INTCON,T0IF
	goto	ServiceTimer0
	goto	ExitISR
ServiceTimer0
	bcf	INTCON,T0IF	    ; clear the interrupt flag. (must be done in software)
	bsf	Flags,TicFlag	; signal the main routine that the Timer has expired
	movlw	Tload		; TLoad --> w
	movwf	TMR0		; w --> TMR0
	goto	ExitISR
ExitISR
	movf	STATUS_Save,w
	movwf	STATUS
	swapf	W_Save,f
	swapf	W_Save,w
	retfie
;-------------------------------------------------------------------------------------------
Start
    BANKSEL OSCCON
    movlw B'11111100';  Bit 7:  SPLLEN: software PLL Enable bit
                     ;  PLLEN==1 --> ignore SPLLEN
                     ;  PLLEN==0 --> SPLLEN==1:Enable, SPLLEN==0:Disable
                     ;  Bit 6-3: IRCF<3:0> Internal Oscillator Frequency Select bit
                     ;  1111: 16MHz HF or 32MHz HF (SPPLEN==1)
                     ;  Bit 2: Unimplemented
                     ;  Bit 1-0: System Clock select bit
                     ;  00: Clock determined by FOSC<2:0> in configuration words

    movwf OSCCON
    BANKSEL TRISC   ; bank1
	bcf	TRISC,2		; Make RC2 is output
	movlw	B'10000011'	;Bit 2-0 PS<2:0>: config Prescaler on Timer0, 011: rate 1:16
                        ;Bit 3: PSA:Prescaler Assignment bit
                        ;    0: Prescaler is assigned to the Timer0 module
                        ;Bit 4: TMR0SE:Timer0 Source Edge Select bit
                        ;    0:Increment on low-to-high transition on T0CKI pin
                        ;    1:Increment on high-to-low transition on T0CKI pin
                        ;Bit 5: TMR0CS:Timer0 Clock Source Select bit
                        ;    1: Transition on T0CKI pin
                        ;    0: Internal instruction cycle clock (FOSC/4)
                        ;Bit 6: INTEDG:Interrupt Edge Select bit
                        ;    0: Interrupt on falling edge of INT pin
                        ;    1:Interrupt on rising edge of INT pin
                        ;Bit 7: Weak Pull-Up Enable bit
                        ;     1= All weak pull-ups are disabled
	movwf OPTION_REG	; configure , bank1
	;movlw	0		; All Ports digital
	;movwf	CMCON		;
    BANKSEL TMR0    ; Bank0
    movlw	Tload		; TLoad --> w
	movwf	TMR0		; w --> TMR0
    bcf	STATUS,0
	bcf	STATUS,1
    bcf	STATUS,2
	;movlw	0b10100000	; enable Timer 0 and global interrupts
	;movwf	INTCON
    bsf INTCON,7
    bsf INTCON,5
    BANKSEL PORTC
    movlw   TicLoad
    movf   TicCnt,w
    bsf    Outport,2
;------------------ ADC configuration --------------------------------------
    ;Conversion start & polling for completion
    ; are included.
    ;
    BANKSEL ADCON1 ;
    MOVLW B'11110000' ;Bit 7: ADFM = 1: 2?s complement
                     ;       ADFM = 0: Sign-magnitude result format
                     ;Bit 6-4: ADCS<2:0> :ADC Conversion Clock Select bits
                     ;111:Frc-->lock supplied from a dedicated FRC oscillato
                     ;Bit 3: Unimplemented
                     ;Bit 2: ADNREF: ADC Negative Voltage Reference Configuration bit
                     ;1 =VREF- is connected to external VREF- pin
                     ;0 =VREF- is connected to VSS
                     ;Bit 1-0: ADPREF<1:0>: ADC Positive Voltage Reference Configuration bits
                     ;11 =VREF+ is connected internally to FVR Buffer 1
                     ;10 = Reserved
                     ;01 =VREF+ is connected to VREF+ pin
                     ;00 =VREF+ is connected to VDD
    ;clock
    MOVWF ADCON1 ;Vdd and Vss Vref
    MOVLW B'00001111' ;
                      ;Bit 7-4:TRIGSEL<3:0>: ADC Auto-conversion Trigger Source Selection bits
                      ;0000: Disable
                      ;Bit 3-0 CHSN<3:0>: Negative Differential Input Channel Select bits
                      ;1111 = ADC Negative reference ? selected by ADNREF

    MOVWF ADCON2 ;to negative

    ;reference
    BANKSEL TRISA ;
    BSF TRISA,1 ;Set RA1 to input
    BANKSEL ANSELA ;
    BSF ANSELA,1 ;Set RA1 t0 analog
    BANKSEL ADCON0 ;
    MOVLW B'00000101' ;Bit 0: ADON =1
                      ;Bit 1: GO/DONE: ADC Conversion Status bit
                      ;       1: in progress
                      ;       0: completed
                      ;Clear by hardware
                      ;Bit 6-2: CHS<4:0>: Positive Differential Input Channel Select bits
                      ;         00001-->AN1
    MOVWF ADCON0 ;Turn ADC On
    ;CALL SampleTime ;Acquisiton delay
    MOVLW 0x64       ;
    MOVWF Delay_100

Delay_Loop
    decfsz  Delay_100         ; decrement Delay counter
    goto    Delay_Loop        ; if Delay counter is not zero, repeat loop
    nop
    
    BSF ADCON0,1 ;Start conversion
    ;BTFSC ADCON0,ADGO ;Is conversion done?
    ;GOTO $-1 ;No, test again
;-------------------------------------------------------------------------------

    ; ----------- Complementary Single-phase PWM PSMC setup --------------------
    ; Fully synchronous operation
    ; Period = 10 us
    ; Duty cycle = 50%
    ; Deadband = 93.75 +15.6/-0 ns
    BANKSEL PSMC1CON
    MOVLW 0x02 ; set period
    MOVWF PSMC1PRH
    MOVLW 0x80
    MOVWF PSMC1PRL
    MOVLW 0x01 ; set duty cycle
    MOVWF PSMC1DCH
    MOVLW 0x40
    MOVWF PSMC1DCL
    CLRF PSMC1PHH ; no phase offset
    CLRF PSMC1PHL
    MOVLW 0x01 ; PSMC clock=64 MHz
    MOVWF PSMC1CLK
    ; output on A, normal polarity
    MOVLW B'00000011'; A and B enables
    MOVWF PSMC1OEN
    MOVWF PSMC1STR0
    CLRF PSMC1POL
    ; set time base as source for all events
    BSF PSMC1PRS, P1PRST
    BSF PSMC1PHS, P1PHST
    BSF PSMC1DCS, P1DCST
    ; set rising and falling dead-band times
    MOVLW 0x06
    MOVWF PSMC1DBR
    MOVWF PSMC1DBF
    ; enable PSMC in Complementary Single Mode
    ; this also loads steering and time buffers
    ; and enables rising and falling deadbands
    MOVLW B'11110001'
    MOVWF PSMC1CON
    BANKSEL TRISC
    BCF TRISC, 0 ; enable pin drivers
    BCF TRISC, 1
    BANKSEL ANSELC
    movlw	B'00000000'
    movwf   ANSELC
    ;---------------------------------------------------------------------------


MainLoop
;Loop
Tictac

    bcf	    Flags,SecFlag       ; clear Second flag
    btfss	Flags,TicFlag	    ; did the Timer0 overflow?
    goto	Tictac_e
;Timer set
    bcf	    Flags,TicFlag	; clear the flag
    decfsz  TicCnt,f        ; Decrement f, Skip if 0
    goto    Tictac_e
    BANKSEL TicCnt          ; Bank0
    movlw	TicLoad
	movwf	TicCnt

    ;---------------------------------------------------------------------------
    ;-------Read ADC------------------------------------------------------------
    BANKSEL ADRESH ;
    BTFSC  ADCON0,1; Bit Test f, Skip if Clear
    goto Blinking
    MOVF ADRESH,W    ;Read upper 2 bits
    MOVWF ADC_AN1_H  ;store in GPR space
    MOVF ADRESL,W    ;Read lower 8 bits
    MOVWF ADC_AN1_L  ;store in GPR space
    BSF ADCON0,1 ;Start conversion
    ;---------------------------------------------------------------------------
    ;------Update PWM frequency-------------------------------------------------
    ; Period = 640 + 61*(ADC_Lo +2^8*ADC_Hi)
    ;61*ADC_Lo
    MOVLW 0x3D  ; put 61 to WREG
    MOVWF  Op1  ; WREG --> Operater 1
    MOVF ADRESL,w ; ADRESL --> WREG
    MOVWF  Op2  ; WREG --> Operater 1
    Mul Op1,Op2 ; result in OutH:OutL

    ; copy OutH:OutL --> TempH:TempL
    MOVF  OutL,w  ; OutL-->WREG
    MOVWF TempL ; WREF-->TempL

    MOVF  OutH,w  ; OutH-->WREG
    MOVWF TempH ; WREG-->TempH

    ;61*ADC_Hi (only 2 lower bit may #0)--> OutH == 0 always
    MOVLW 0x3D  ; put 61 to WREG
    MOVWF  Op1  ; WREG --> Operater 1
    MOVF ADRESH,w ; ADRESL --> WREG
    MOVWF  Op2  ; WREG --> Operater 1
    Mul Op1,Op2 ; OutH==0,  result --> OutL

    ; 2^8*61*ADC_Hi + 61*ADC_Lo
    MOVF  OutL,w  ;
    ADDWF TempH,1 ;carrier flag may be set here
    bcf STATUS,0        ; clear carry bit

    ; 640 = 0x280, 640 + 2^8*61*AHC_Hi + 61*ADC_Lo
    MOVLW 0x80
    ADDWF TempL,1  ;carrier flag may be set here
    MOVLW 0x02
    ADDWFC TempH,1

    ; move to PSMC register
    BANKSEL PSMC1CON
    MOVF TempL,w
    MOVWF PSMC1PRL
    MOVF TempH,w
    MOVWF PSMC1PRH

    LSRF TempL
    MOVF TempL,w
    MOVWF PSMC1DCL

    LSRF TempH
    MOVF TempH,w
    MOVWF PSMC1DCH

    bsf PSMC1CON,6 ; set PSMC1LD
                   ; It is clear by Hardware
    ;---------------------------------------------------------------------------
Blinking
    ;-------Blinking Led : pin RC2 ---------------------------------------------
    BANKSEL PORTC
	movlw	0x04	;blink bit 2
	xorwf	Outport

    ;movfw   Outport
    ;bcf     Outport,2
    bsf     Flags,SecFlag ;Set Second flag

Tictac_e

goto MainLoop
;-------------------------------------------------------------------------------
end
