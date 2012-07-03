/**********************************************************************
* 2009 Microchip Technology Inc.
*
* FileName:        	i2c_master.c
* Dependencies:    Header (.h) files if applicable, see below
* Processor:       	PIC24F
* Compiler:        	MPLAB C30 v3.11 or higher
*
* SOFTWARE LICENSE AGREEMENT:
* Microchip Technology Incorporated ("Microchip") retains all 
* ownership and intellectual property rights in the code accompanying
* this message and in all derivatives hereto.  You may use this code,
* and any derivatives created by any person or entity by or on your 
* behalf, exclusively with Microchip's proprietary products.  Your 
* acceptance and/or use of this code constitutes agreement to the 
* terms and conditions of this notice.
*
* CODE ACCOMPANYING THIS MESSAGE IS SUPPLIED BY MICROCHIP "AS IS". NO 
* WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT 
* NOT LIMITED TO, IMPLIED WARRANTIES OF NON-INFRINGEMENT, 
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS 
* CODE, ITS INTERACTION WITH MICROCHIP'S PRODUCTS, COMBINATION WITH 
* ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
*
* YOU ACKNOWLEDGE AND AGREE THAT, IN NO EVENT, SHALL MICROCHIP BE 
* LIABLE, WHETHER IN CONTRACT, WARRANTY, TORT (INCLUDING NEGLIGENCE OR
* BREACH OF STATUTORY DUTY), STRICT LIABILITY, INDEMNITY, 
* CONTRIBUTION, OR OTHERWISE, FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
* EXEMPLARY, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, FOR COST OR 
* EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE CODE, HOWSOEVER 
* CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE
* DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT ALLOWABLE BY LAW, 
* MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS
* CODE, SHALL NOT EXCEED THE PRICE YOU PAID DIRECTLY TO MICROCHIP 
* SPECIFICALLY TO HAVE THIS CODE DEVELOPED.
*
* You agree that you are solely responsible for testing the code and 
* determining its suitability.  Microchip has no obligation to modify,
* test, certify, or support the code.
*
* REVISION HISTORY:
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author        	Date      	Comments on this revision
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Harsha.J.M	04/04/09	First release of source file
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
* ADDITIONAL NOTES:
* Code uses the PERIPHERAL LIBRARY support available with MPLAB C30 Compiler
* Code Tested on:
* EXPLORER 16 demo board with PIC24FJ256GB110 controller
* The Processor starts with the External Crystal (8 Mhz).
*
* I2C Master is configured on  I2C1 available on device
* I2C MASTER: 	I2C1 Module
*
*Connections:
*		I2C1 (MASTER)										I2Cx (SLAVE)
*														(Another Device)
*		SCL1 (pin 66)	<----------Pull up resister (4.7K) to Vcc--------------->	SCLx
*		SDA1 (pin 67)	<----------Pull up resister (4.7K) to Vcc--------------->	SDAx
*
* Pull up resister of 4.7K is necessary to tie the lines connecting SCL1 & SCLx to Vcc.
* Pull up resister of 4.7K is necessary to tie the lines connecting SDA1 & SDAx to Vcc.
**********************************************************************/
#define USE_AND_OR
#include "p24Fxxxx.h"
#include "i2c.h"

#if defined(__PIC24FJ256GB110__)
/*************** COFIGURATION **************************************
*	Watchdog Timer Disabled
*	Two Speed Start-up enabled
*	Oscillator Selection: HS oscillator ( 8MHz crystal on EXPLORER 16 Board )
*	Clock switching and clock monitor both enabled
********************************************************************/
_CONFIG1(FWDTEN_OFF & ICS_PGx2)
_CONFIG2(IESO_ON & FNOSC_PRI & FCKSM_CSECME & POSCMOD_HS )	//
#endif

unsigned char I2C_RECV[15],I2C_TRANS[] = "MICROCHIP";

//******************** Interrupt sevice rotuine for I2C1 Master ***************************
void __attribute__((interrupt,no_auto_psv)) _MI2C1Interrupt(void)
{
  MI2C1_Clear_Intr_Status_Bit;							//Clear Interrupt status of I2C1		
}


int main(void)
{
   unsigned int config1 = 0,i=0;
   unsigned int config2 = 0;
   
   /* Turn off I2C modules */
   CloseI2C1();											//Disbale I2C1 mdolue if enabled previously
    
 
//************ I2C interrupt configuration ****************************************************** 
	ConfigIntI2C1(MI2C_INT_OFF);						//Disable I2C interrupt
     
//***************** I2C1 configuration **********************************************************
/**********************************************************************************************
*
*		I2C1 enabled
*		continue I2C module in Idle mode
*		IPMI mode not enabled
*		I2CADD is 7-bit address
*		Disable Slew Rate Control for 100KHz
*		Enable SM bus specification
*		Disable General call address
*		Baud @ 8MHz = 39 into I2CxBRG
**********************************************************************************************/
   config1 = (I2C_ON  | I2C_7BIT_ADD );
   config2 = 39;
   OpenI2C1(config1,config2);						//configure I2C1

   IdleI2C1();
   StartI2C1();
   while(I2C1CONbits.SEN );  //Wait till Start sequence is completed
   MI2C1_Clear_Intr_Status_Bit; //Clear interrupt flag

   MasterWriteI2C1(0x40); //Write Slave address and set master for transmission
   while(I2C1STATbits.TBF); //Wait till address is transmitted
   while(!IFS1bits.MI2C1IF); //Wait for ninth clock cycle
   MI2C1_Clear_Intr_Status_Bit; //Clear interrupt flag
   while(I2C1STATbits.ACKSTAT);

	for(i=0;i<1000;i++);
//************** Master Data transmission after slave ACK ********************************************   
   MasterputsI2C1(I2C_TRANS); 						//Transmit string of data

//************* Restart condition for further reception from master ************************************
  
	 IdleI2C1();										//wait for the I2C to be Idle	
   RestartI2C1();									//Restart signal
   while(I2C1CONbits.RSEN ); 						//Wait till Restart sequence is completed	
   for(i=0;i<1000;i++);
//*************** Master sends Slave Address with read signal *****************************************
		Nop();
	MI2C1_Clear_Intr_Status_Bit;
   MasterWriteI2C1(0x41); 							//Write Slave address and set master for reception
   while(!IFS1bits.MI2C1IF); 						//Wait for ninth clock cycle
   while(I2C1STATbits.ACKSTAT);						//check for ACK from slave
	MI2C1_Clear_Intr_Status_Bit;
//************** Master Recieves from slave ********************************************************
   MastergetsI2C1(10,I2C_RECV,1000);				//Master recieves from Slave upto 10 bytes

   
//************** Stop condition *******************************************************************  
   IdleI2C1();										//wait for the I2C to be Idle	
   StopI2C1();										//Terminate communication protocol with stop signal
   while(I2C1CONbits.PEN); 							//Wait till stop sequence is completed
   
   CloseI2C1();										//Disable I2C
   
   while(1);										//end of program
}


