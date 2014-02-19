/**********************************************************************
* © 2007 Microchip Technology Inc.
*
* FileName:        ADCChannelDrv.h
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

#ifndef __ADCCHANNELDRV_H__
#define __ADCCHANNELDRV_H__

#define FCY		40000000
#define FSAMP		8000		/* Sampling Frequency	*/
#define BUFFER_SIZE	20		/* This is the size of each buffer	*/
#define ADC_CHANNEL_DMA_BUFSIZE (BUFFER_SIZE*2)	
#define ADC_CHANNEL_BUFSIZE (BUFFER_SIZE*2)	
#define DMA_BASE_ADDR 0x7800
#include <p33FJ256GP710A.h>


typedef struct sADCChannelHandle {
	int * buffer1;
	int * buffer2;
	volatile int bufferIndicator;
	volatile int isReadBusy;
	
}ADCChannelHandle;


void ADCChannelInit	(ADCChannelHandle * pHandle,int * pBufferInDMA);
void ADCChannelStart	(ADCChannelHandle * pHandle);
void ADCChannelRead	(ADCChannelHandle * pHandle,int *buffer,int size);
int ADCChannelIsBusy	(ADCChannelHandle * pHandle);

#define ADCON1VAL 		0x0744     /* These values will be loaded in 	*/    
#define ADCON2VAL 		0x0000     /* init call						*/	
#define ADCON3VAL		0x0010          
#define ADCHSVAL			0x0000			  
#define ADPCFGVAL		0xFFFE		  
#define ADCSSLVAL		0x0000 


#endif


