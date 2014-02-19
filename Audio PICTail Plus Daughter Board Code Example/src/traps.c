/**********************************************************************
* © 2007 Microchip Technology Inc.
*
* FileName:        traps.c
* Dependencies:    Header (.h) files if applicable, see below
* Processor:       dsPIC33Fxxxx/PIC24Hxxxx
* Compiler:        MPLAB® C30 v3.00 or higher
*
* SOFTWARE LICENSE AGREEMENT:
* Microchip Technology Incorporated ("Microchip") retains all ownership and 
* intellectual property rights in the code accompanying this message and in all 
* derivatives hereto.  You may use this code, and any derivatives created by 
* any person or entity by or on your behalf, exclusively with Microchip,s 
* proprietary products.  Your acceptance and/or use of this code constitutes 
* agreement to the terms and conditions of this notice.
*
* CODE ACCOMPANYING THIS MESSAGE IS SUPPLIED BY MICROCHIP "AS IS".  NO 
* WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED 
* TO, IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A 
* PARTICULAR PURPOSE APPLY TO THIS CODE, ITS INTERACTION WITH MICROCHIP,S 
* PRODUCTS, COMBINATION WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
*
* YOU ACKNOWLEDGE AND AGREE THAT, IN NO EVENT, SHALL MICROCHIP BE LIABLE, WHETHER 
* IN CONTRACT, WARRANTY, TORT (INCLUDING NEGLIGENCE OR BREACH OF STATUTORY DUTY), 
* STRICT LIABILITY, INDEMNITY, CONTRIBUTION, OR OTHERWISE, FOR ANY INDIRECT, SPECIAL, 
* PUNITIVE, EXEMPLARY, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, FOR COST OR EXPENSE OF 
* ANY KIND WHATSOEVER RELATED TO THE CODE, HOWSOEVER CAUSED, EVEN IF MICROCHIP HAS BEEN 
* ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT 
* ALLOWABLE BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO 
* THIS CODE, SHALL NOT EXCEED THE PRICE YOU PAID DIRECTLY TO MICROCHIP SPECIFICALLY TO 
* HAVE THIS CODE DEVELOPED.
*
* You agree that you are solely responsible for testing the code and 
* determining its suitability.  Microchip has no obligation to modify, test, 
* certify, or support the code.
*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* ADDITIONAL NOTES:
* Code Tested on:
* Explorer16 development board and ADC/PWM PICTail Board
**********************************************************************/


#include <p33FJ256GP710A.h>


/******************************************************************
* Standard Exception Vector handlers if #include <p33FJ256GP710.h>ALTIVT (INTCON2<15>) = 0  *
*                                                                 *
* Not required for labs but good to always include                *
******************************************************************/
void __attribute__((__interrupt__,no_auto_psv)) _OscillatorFail(void)
{

        INTCON1bits.OSCFAIL = 0;
        while(1);
}

void __attribute__((__interrupt__,no_auto_psv)) _AddressError(void)
{
	Nop();
	Nop();
	Nop();
        INTCON1bits.ADDRERR = 0;
        while(1);
}

void __attribute__((__interrupt__,no_auto_psv)) _StackError(void)
{
	
        INTCON1bits.STKERR = 0;
        while(1);
}

void __attribute__((__interrupt__,no_auto_psv)) _MathError(void)
{
	Nop();
	Nop();
	Nop();
        INTCON1bits.MATHERR = 0;
        while(1);
}

/*void __attribute__((__interrupt__,no_auto_psv)) _DMACError(void)
{

        INTCON1bits.DMACERR = 0;
        while(1);
}*/

/******************************************************************
* Alternate Exception Vector handlers if ALTIVT (INTCON2<15>) = 1 *
*                                                                 *
* Not required for labs but good to always include                *
******************************************************************/
void __attribute__((__interrupt__,no_auto_psv)) _AltOscillatorFail(void)
{

        INTCON1bits.OSCFAIL = 0;
        while(1);
}

void __attribute__((__interrupt__,no_auto_psv)) _AltAddressError(void)
{

        INTCON1bits.ADDRERR = 0;
        while(1);
}

void __attribute__((__interrupt__,no_auto_psv)) _AltStackError(void)
{

        INTCON1bits.STKERR = 0;
        while(1);
}

void __attribute__((__interrupt__,no_auto_psv)) _AltMathError(void)
{

        INTCON1bits.MATHERR = 0;
        while(1);
}



