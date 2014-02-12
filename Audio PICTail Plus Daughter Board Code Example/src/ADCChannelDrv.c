/**********************************************************************
* © 2007 Microchip Technology Inc.
*
* FileName:        ADCChannelDrv.c
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

#include "..\h\ADCChannelDrv.h"
static ADCChannelHandle * thisADCChannel;


/*=============================================================================
 ADC, DMA0 and TIMER 3  Modules Initializations
=============================================================================*/

void ADCChannelInit	(ADCChannelHandle * pHandle,int * pBufferInDMA)
{
	/* This function will intialize the DMA		*/
	/* DMA0 is used to read from the ADC		*/
	

	thisADCChannel = pHandle;
	pHandle->buffer1 	= pBufferInDMA;	/* Assign the ping pong buffers for the ADC DMA*/
	pHandle->buffer2 	= (int *)(pBufferInDMA + BUFFER_SIZE);

	
	DMA0CONbits.SIZE	= 0;		/* Word transfers				*/
	DMA0CONbits.DIR		= 0;		/* From ADC1BUF to DMA				*/
	DMA0CONbits.HALF 	= 0;		/* Interrupt when all the data has been moved	*/
	DMA0CONbits.NULLW 	= 0;		/* No NULL writes - Normal Operation		*/
	DMA0CONbits.AMODE 	= 0;		/* Register Indirect with post-increment mode	*/
	DMA0CONbits.MODE 	= 2;		/* Continuous  ping pong mode enabled		*/
	
	DMA0REQbits.FORCE 	= 0;		/* Automatic transfer				*/
	DMA0REQbits.IRQSEL	= 0xD;		/* Codec transfer don				*/
	
	DMA0STA	= (int)(pHandle->buffer1) - (int)&_DMA_BASE;
	DMA0STB = (int)(pHandle->buffer2) - (int)&_DMA_BASE;
	
	
	DMA0PAD	= (int )&ADC1BUF0;
	DMA0CNT	= BUFFER_SIZE - 1;

	AD1CON1 	= ADCON1VAL;    /* Load the ADC registers with  value 	*/                            
	AD1CON2 	= ADCON2VAL;    /* specified in ADCChannelDrv.h	*/ 
	AD1CON3 	= ADCON3VAL;                              
	AD1CHS0 	= ADCHSVAL;                              
	AD1PCFGL 	= ADPCFGVAL;           
	AD1CSSL 	= ADCSSLVAL;

	TMR3 		= 0;
	PR3		= (FCY/FSAMP) - 1;

}

/*=============================================================================
Enable the ADC, DMA0 and TIMER 3  Modules
=============================================================================*/

void ADCChannelStart	(ADCChannelHandle * pHandle)
{
	pHandle->bufferIndicator 	= 0;
	pHandle->isReadBusy 		= 1;
	_DMA0IF = 0;
	_DMA0IE = 1;
	DMA0CONbits.CHEN = 1;		/* Enable the DMA Channel	*/
	AD1CON1bits.ADON = 1;		/* Enable ADC module		*/
	T3CONbits.TON 	 = 1;		/* Enable Timer 3 	    	*/

}
/*=============================================================================
Read the data from DMA0 Buffers and perform ping pong Buffer management
=============================================================================*/

void ADCChannelRead	(ADCChannelHandle * pHandle,int *data,int size)
{
	int	*source;
	int 	i;

	/* if the buffer indicator bit is 1, then use buffer 1 else use buffer2	*/
	/* Since the DMA ping pongs between these buffer, you must know */
	/* which one to read. The bufferIndicators keep track of this		*/
	
	source = (pHandle->bufferIndicator) ? pHandle->buffer1 : pHandle->buffer2;
	if (size > BUFFER_SIZE) size = BUFFER_SIZE;

	
	for(i = 0;i < size;i++)
	{
		data[i] = source[i];

	}
	pHandle->isReadBusy = 1;

}

/*=============================================================================
Return value from this function indicates if a new frame is available
=============================================================================*/


int ADCChannelIsBusy	(ADCChannelHandle * pHandle)
{
	return(pHandle->isReadBusy);
}

/*=============================================================================
DMA0 Interrupt Service Rouine
=============================================================================*/


void __attribute__((__interrupt__,no_auto_psv)) _DMA0Interrupt(void)
{
	_DMA0IF = 0;
	thisADCChannel->bufferIndicator  	^= 1;		/* Flip the indicator bit		*/
	thisADCChannel->isReadBusy		= 0;			/* New frame is available	*/
}
