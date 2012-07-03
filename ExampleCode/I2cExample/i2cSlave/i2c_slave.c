/**********************************************************************
* 2009 Microchip Technology Inc.
*
* FileName:        	i2c_slave.c
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
* I2C Slave is configured on  I2C2 available on  device
* I2C SLAVE:	I2C2 Module
*
*Connections:
*		I2Cx (MASTER)										I2C2 (SLAVE)
*		(Another Device)
*		SCLx		 	<----------Pull up resister (4.7K) to Vcc--------------->	SCL2  (pin 58)
*		SDAx 		<----------Pull up resister (4.7K) to Vcc--------------->	SDA2  (pin 59)
*
* Pull up resister of 4.7K is necessary to tie the lines connecting SCLx & SCL2 to Vcc.
* Pull up resister of 4.7K is necessary to tie the lines connecting SDAx & SDA2 to Vcc.
**********************************************************************/
#define USE_AND_OR
#include "p24fxxxx.h"
#include "i2c.h"

#if defined(__PIC24FJ256GB110__)
/*************** COFIGURATION **************************************
*	Watchdog Timer Disabled
*	Two Speed Start-up enabled
*	Oscillator Selection: HS oscillator ( 8MHz crystal on EXPLORER 16 Board )
*	Clock switching and clock monitor both enabled
********************************************************************/
_CONFIG1(FWDTEN_OFF & ICS_PGx2)
_CONFIG2(IESO_ON & FNOSC_PRI & FCKSM_CSECME & POSCMOD_HS & PLLDIV_DIV2 & IOL1WAY_OFF)
#endif


unsigned char I2C_RECV[15],I2C_TRANS[] = "I2C_MODULE";



//******************** Interrupt sevice rotuine for I2C2 Slave ***************************
void __attribute__((interrupt,no_auto_psv)) _SI2C2Interrupt(void)
{
  SI2C2_Clear_Intr_Status_Bit;							//Clear Interrupt status of I2C2
}

int main(void)
{
   unsigned int config1 = 0;
   unsigned int config2 = 0;
   unsigned int temp=0,i=0;
   
for(i=0;i<15;i++)
I2C_RECV[i]=0;

   /* Turn off I2C modules */
   CloseI2C2();											//Disbale I2C2 mdolue if enabled previously 
    
 
//************ I2C interrupt configuration ****************************************************** 
	ConfigIntI2C2(SI2C_INT_OFF);						//Disable I2C slave Interrupt     
//***************** I2C2 configuration **********************************************************
/**********************************************************************************************
*
*		I2C2 enabled
*		continue I2C module in Idle mode
*		IPMI mode not enabled
*		I2CADD is 7-bit address
*		Disable Slew Rate Control for 100KHz
*		Enable SM bus specification
*		Disable General call address
*		Baud @ 8MHz = 39 into I2CxBRG
**********************************************************************************************/
   config1 = (I2C_ON | I2C_7BIT_ADD | I2C_CLK_REL | I2C_STR_EN  );
   config2 = 0;										//This defines I2C to be slave
   OpenI2C2(config1,config2);						//configure I2C2
   I2C2ADD = 0x20;									//I2C address set to 1E		
   
	while(IFS3bits.SI2C2IF==0);						//wait untill the data is transmitted from master
	SI2C1_Clear_Intr_Status_Bit;
	temp = SlaveReadI2C2();							//Read address to empty buffer
   
//********************* Data reception from slave ******************************************
	SlavegetsI2C2(I2C_RECV,1000);


//********************* After restart condition wait till the address with read signal is sent ****************		

	while(IFS3bits.SI2C2IF==0);						//wait untill the data is transmitted from master
	SI2C1_Clear_Intr_Status_Bit;
	temp = SlaveReadI2C2();							//Read address to empty buffer

		
	if(I2C2STATbits.R_W)
	{
	SlaveputsI2C2(I2C_TRANS);					//Slave transmission

	}
		
		
   CloseI2C2();										//Disable I2C
   
   while(1);										//end of program
}


