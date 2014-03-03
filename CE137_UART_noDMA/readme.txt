*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author            Date      Comments on this revision
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Rob Welch         11/28/06    Original Release
*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* ADDITIONAL NOTES: 
* Small, bare bones program to guide designers with UART implementation
*  To run program, plug DB9 cable to Terminal (9600,8,N,1) - 
*	Data sent from TERMINAL END will be represented as binary equivalent
*	of ASCII on 7 LSB LEDs of HPC16 Explorer board
*
*	Data is sent from HPC16 by pressing S3 - S6.  A single letter will
*	be transmitted upon each keypress.
*
* Peripheral Library was not utilized, Bit Addressing was used in order
* to show all details involved in initialization of UART.
*
**********************************************************************/
