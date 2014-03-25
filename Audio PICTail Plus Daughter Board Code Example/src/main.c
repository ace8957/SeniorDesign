/**********************************************************************
* © 2007 Microchip Technology Inc.
*
* FileName:        main.c
* Dependencies:    Header (.h) files if applicable, see below
* Processor:       dsPIC33Fxxxx/PIC24Hxxxx
* Compiler:        MPLABÆ C30 v3.00 or higher
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

//#include <p33FJ256GP710A.h>
//#include "../h/config.h"

#include "ADCChannelDrv.h"
#include "OCPWMDrv.h"
#include "G711.h"

#include "CONU2.h"
#include "MyQueue.h"

#include "AES.h"

#include <string.h>

#define FRAME 20
_FGS(GWRP_OFF & GCP_OFF);
_FOSCSEL(FNOSC_FRC);
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF & POSCMD_XT);
_FWDT(FWDTEN_OFF);

//from serial
//_FOSCSEL(FNOSC_FRC); // Internal FRC oscillator
//_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF & POSCMD_NONE);

int adcBuffer		[ ADC_CHANNEL_DMA_BUFSIZE] __attribute__((space(dma)));
int ocPWMBuffer	[OCPWM_DMA_BUFSIZE]	__attribute__((space(dma)));

int 	inputFrame		[FRAME];
char encodedFrame	[FRAME];
int 	decodedFrame		[FRAME];

ADCChannelHandle 	adcChannelHandle;
OCPWMHandle 		ocPWMHandle;

ADCChannelHandle *pADCChannelHandle 	= &adcChannelHandle;
OCPWMHandle 	*pOCPWMHandle 		= &ocPWMHandle;


void __attribute__((interrupt, no_auto_psv)) _U2RXInterrupt(void) {
    char test = U2RXREG;
    //LATA = test;
    push(test);
    IFS1bits.U2RXIF = 0;
}

void __attribute__((interrupt, no_auto_psv)) _U2TXInterrupt(void) {
    IFS1bits.U2TXIF = 0;
}

void InitClock() {
    PLLFBD = 38; // M = 40
    CLKDIVbits.PLLPOST = 0; // N1 = 2
    CLKDIVbits.PLLPRE = 0; // N2 = 2
    OSCTUN = 0;
    RCONbits.SWDTEN = 0;

    // Clock switch to incorporate PLL
    __builtin_write_OSCCONH(0x01); // Initiate Clock Switch to
    // FRC with PLL (NOSC=0b001)
    __builtin_write_OSCCONL(0x01); // Start clock switching
    while (OSCCONbits.COSC != 0b001); // Wait for Clock switch to occur

    while (OSCCONbits.LOCK != 1) {
    };
}

void InitUART2() {
    // This is an EXAMPLE, so brutal typing goes into explaining all bit sets

    // The HPC16 board has a DB9 connector wired to UART2, so we will
    // be configuring this port only

    // configure U2MODE
    U2MODEbits.UARTEN = 0; // Bit15 TX, RX DISABLED, ENABLE at end of func
    //U2MODEbits.notimplemented;	// Bit14
    U2MODEbits.USIDL = 0; // Bit13 Continue in Idle
    U2MODEbits.IREN = 0; // Bit12 No IR translation
    U2MODEbits.RTSMD = 0; // Bit11 Simplex Mode
    //U2MODEbits.notimplemented;	// Bit10
    U2MODEbits.UEN = 0; // Bits8,9 TX,RX enabled, CTS,RTS not
    U2MODEbits.WAKE = 0; // Bit7 No Wake up (since we don't sleep here)
    U2MODEbits.LPBACK = 0; // Bit6 No Loop Back
    U2MODEbits.ABAUD = 0; // Bit5 No Autobaud (would require sending '55')
    U2MODEbits.URXINV = 0; // Bit4 IdleState = 1  (for dsPIC)
    U2MODEbits.BRGH = 0; // Bit3 16 clocks per bit period
    U2MODEbits.PDSEL = 0; // Bits1,2 8bit, No Parity
    U2MODEbits.STSEL = 0; // Bit0 One Stop Bit

    // Load a value into Baud Rate Generator.  Example is for 9600.
    // See section 19.3.1 of datasheet.
    //  U2BRG = (Fcy/(16*BaudRate))-1
    //  U2BRG = (37M/(16*9600))-1
    //  U2BRG = 240
    U2BRG = 240; // 40Mhz osc, 9600 Baud

    // Load all values in for U1STA SFR
    U2STAbits.UTXISEL1 = 0; //Bit15 Int when Char is transferred (1/2 config!)
    U2STAbits.UTXINV = 0; //Bit14 N/A, IRDA config
    U2STAbits.UTXISEL0 = 0; //Bit13 Other half of Bit15
    //U2STAbits.notimplemented = 0;	//Bit12
    U2STAbits.UTXBRK = 0; //Bit11 Disabled
    U2STAbits.UTXEN = 0; //Bit10 TX pins controlled by periph
    U2STAbits.UTXBF = 0; //Bit9 *Read Only Bit*
    U2STAbits.TRMT = 0; //Bit8 *Read Only bit*
    U2STAbits.URXISEL = 0; //Bits6,7 Int. on character recieved
    U2STAbits.ADDEN = 0; //Bit5 Address Detect Disabled
    U2STAbits.RIDLE = 0; //Bit4 *Read Only Bit*
    U2STAbits.PERR = 0; //Bit3 *Read Only Bit*
    U2STAbits.FERR = 0; //Bit2 *Read Only Bit*
    U2STAbits.OERR = 0; //Bit1 *Read Only Bit*
    U2STAbits.URXDA = 0; //Bit0 *Read Only Bit*

    IPC7 = 0x4400; // Mid Range Interrupt Priority level, no urgent reason

    IFS1bits.U2TXIF = 0; // Clear the Transmit Interrupt Flag
    IEC1bits.U2TXIE = 1; // Enable Transmit Interrupts
    IFS1bits.U2RXIF = 0; // Clear the Recieve Interrupt Flag
    IEC1bits.U2RXIE = 1; // Enable Recieve Interrupts

    U2MODEbits.UARTEN = 1; // And turn the peripheral on

    U2STAbits.UTXEN = 1;
    // I think I have the thing working now.
}



int main(void)
{
        JTAGEN_OFF;
	CLKDIV = 0;
	PLLFBD = 38;

	__builtin_write_OSCCONH(0x03);		/*	Initiate Clock Switch to Primary Oscillator with PLL (NOSC=0b011)*/
	__builtin_write_OSCCONL(0x01);
	while (OSCCONbits.COSC != 0b011);	/*	Wait for Clock switch to occur	*/
	while(!OSCCONbits.LOCK);

        InitClock();
        InitUART2();
        InitQueue();

	ADCChannelInit	(pADCChannelHandle,adcBuffer);	/*Initialize the ADC Channel	*/
	ADCChannelStart	(pADCChannelHandle);			/*Start acquiring samples	*/
	OCPWMInit		(pOCPWMHandle,ocPWMBuffer);	/*Initialize the OC module 	*/
	OCPWMStart		(pOCPWMHandle);				/*Start playback of samples	*/

        

	while(1)
	{
            /* Wait till a frame of input data is available	*/
           // while(ADCChannelIsBusy(pADCChannelHandle));

            /* Read the data into inputBuffer	*/
          //  ADCChannelRead	(pADCChannelHandle,inputFrame,FRAME);
            /*Encode the frame	*/
            
            //G711Lin2Ulaw(inputFrame,encodedFrame,FRAME);
            /* Decode the frame	*/
            //G711Ulaw2Lin(encodedFrame,decodedFrame,FRAME);
            /* Wait till the OC is available for new a frame	*/
            int key[8] = {0x0100,0x0302,0x0504,0x0706,0x0908,0x0B0A,0x0D0C,0x0F0E};
//            AESEncrypt((int*)inputFrame, key);
//            AESCalcDecKey(key);
//            AESDecrypt((int*)inputFrame, key);
            /* check for receive errors */
            char ReceivedChar;
            if (U2STAbits.FERR == 1) {
                continue;
            }
            /* must clear the overrun error to keep uart receiving */
            if (U2STAbits.OERR == 1) {
                U2STAbits.OERR = 0;
                continue;
            }
            /* get the data */
            int track=0;
            int done = 0;
            TRISA = 0;
            PORTA = 0x00;
            while(!done) {

                PORTA  = 0x00;
                ReceivedChar = pop();

                if(ReceivedChar == 0){
                    continue;
                }
                if(ReceivedChar == 'S' && track == 0){
                    track++;
                }
                else if(ReceivedChar == 't' && track == 1) {
                    track++;
                    //PORTA = 0x02;
                }
                else if(ReceivedChar == 'a' && track == 2) {
                    track++;
                    //PORTA = 0x03;
                }
                else if(ReceivedChar == 'r' && track == 3) {
                    track++;
                    //PORTA = 0x04;
                }
                else if(ReceivedChar == 't' && track == 4) {
                    track++;
                    //PORTA = 0x05;
                }
                else if(track == 5){
                    //should now be at the block stuffs
                    //putsU2("ACK");
                    //PORTA = 0xFF;
                    char buffer[16] = {'\0'};
                    PORTA = 0x0;
                    int count = 0;
                    while(1){
                        ReceivedChar = pop();
                        if(ReceivedChar == 0){
                            continue;
                        }

                        buffer[count] = ReceivedChar;
                        count++;
//                        char let = 'a';
//                        if(ReceivedChar == ++let){
//                            count++;
//
//                        }
                        
//                        //if(let == 't')
                        if(count > 15){
                            //count = 0;
                            AESCalcDecKey(key);
                            AESDecrypt((int*)buffer, key);
                            
                            char let = 'a';
                            int i=0;
                            for(i;i<16;i++){
                                if(let == buffer[i]){
                                    let++;
                                }
                                    
                                else{
                                    PORTA = 0x0F;
                                    while(1);
                                }

                            }
//                            char expected[20] = "abcdefghijklmnopqrst";
//
//                            if(!memcmp(buffer,(int*)expected, 20)){
//                                count++;
//                                if(count == 19){
                                    PORTA = 0xFF;
                                    while(1);
//                                } else{
//                                    PORTA = 0x00;
//                                    while(1);
//                                }

                            } else{
                                PORTA = 0xF0; // error
//                                while(1);
                            }
                            
                            
                        }
                    }
                    /*
                    int count = 0;
                    while(count < FRAME){
                        ReceivedChar = pop();
                        if(ReceivedChar == 0){
                            continue;
                        }
                        inputFrame[count] = ReceivedChar;
                        count++;
                    }
                    done = 1;
                     */
                    //PORTA = 0X00;
                }
//                else{
//                    track = 0;
//                    PORTA = 0xF0;
//                }
            }




            while(OCPWMIsBusy(pOCPWMHandle));
            /* Write the decoded frame to the output	*/
            //OCPWMWrite (pOCPWMHandle,decodedFrame,FRAME);
            OCPWMWrite (pOCPWMHandle,inputFrame,FRAME);
	

    return 0;
}

