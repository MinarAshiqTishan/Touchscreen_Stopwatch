/******************************************************************************

                          eXtreme Electronics xAPI(TM)
						  ----------------------------
xAPI is a Powerful but easy to use C library to program the xBoard(TM)
series of AVR development board. The library has support for commonly use tasks
like:-

*LCD interfacing
*MUXED 7 segment displays.
*Remote Control
*Serial Communication
*DC Motor Controls with Speed Control
*Analog Measurement for Sensor Interface.
*Temperature Measurement.
*I2C Communication.
*EEPROM Interface
*Real Time Clock (RTC Interface)

The APIs are highly documented and easy to use even by a beginner.

For More Info Log On to 
www.eXtremeElectronics.co.in

Copyright 2008-2009 eXtreme Electronics India

                                    LCD Core
						           ----------
This module is used for interfacing with Standard Alpha Numeric LCD Modules.
For More information please see supplied tutorials and videos.

History
*******
Unknown:
	*Now data lines can begin on any bit of a PORT.[TESTED]

01 Aug 2010 :
	*Added Support for 20x4,16x4 LCDs [TESTED]
	*Corrected Cursor type selection for LCDInit() [TESTED]

15 Sep 2010:
	*Corrected some preprocessor errors.


                                     NOTICE
									--------
NO PART OF THIS WORK CAN BE COPIED, DISTRIBUTED OR PUBLISHED WITHOUT A
WRITTEN PERMISSION FROM EXTREME ELECTRONICS INDIA. THE LIBRARY, NOR ANY PART
OF IT CAN BE USED IN COMMERCIAL APPLICATIONS. IT IS INTENDED TO BE USED FOR
HOBBY, LEARNING AND EDUCATIONAL PURPOSE ONLY. IF YOU WANT TO USE THEM IN 
COMMERCIAL APPLICATION PLEASE WRITE TO THE AUTHOR.


WRITTEN BY:
AVINASH GUPTA
me@avinashgupta.com

*******************************************************************************/

#include <avr/io.h>
#include <inttypes.h>
#include <util/delay.h>

#include "lcd.h"



#define LCD_DATA_PORT 	PORT(LCD_DATA)
#define LCD_E_PORT 		PORT(LCD_E)
#define LCD_RS_PORT 	PORT(LCD_RS)
#define LCD_RW_PORT 	PORT(LCD_RW)

#define LCD_DATA_DDR 	DDR(LCD_DATA)
#define LCD_E_DDR 		DDR(LCD_E)
#define LCD_RS_DDR 		DDR(LCD_RS)
#define LCD_RW_DDR 		DDR(LCD_RW)

#define LCD_DATA_PIN	PIN(LCD_DATA)

#define SET_E() (LCD_E_PORT|=(1<<LCD_E_POS))
#define SET_RS() (LCD_RS_PORT|=(1<<LCD_RS_POS))
#define SET_RW() (LCD_RW_PORT|=(1<<LCD_RW_POS))

#define CLEAR_E() (LCD_E_PORT&=(~(1<<LCD_E_POS)))
#define CLEAR_RS() (LCD_RS_PORT&=(~(1<<LCD_RS_POS)))
#define CLEAR_RW() (LCD_RW_PORT&=(~(1<<LCD_RW_POS)))



void LCDByte(uint8_t c,uint8_t isdata)
{
//Sends a byte to the LCD in 4bit mode
//cmd=0 for data
//cmd=1 for command


//NOTE: THIS FUNCTION RETURS ONLY WHEN LCD HAS PROCESSED THE COMMAND

uint8_t hn,ln;			//Nibbles
uint8_t temp;

hn=c>>4;
ln=(c & 0x0F);

if(isdata==0)
	CLEAR_RS();
else
	SET_RS();

_delay_us(0.500);		//tAS

SET_E();

//Send high nibble

temp=(LCD_DATA_PORT & (~(0X0F<<LCD_DATA_POS)))|((hn<<LCD_DATA_POS));
LCD_DATA_PORT=temp;

_delay_us(1);			//tEH

//Now data lines are stable pull E low for transmission

CLEAR_E();

_delay_us(1);

//Send the lower nibble
SET_E();

temp=(LCD_DATA_PORT & (~(0X0F<<LCD_DATA_POS)))|((ln<<LCD_DATA_POS));

LCD_DATA_PORT=temp;

_delay_us(1);			//tEH

//SEND

CLEAR_E();

_delay_us(1);			//tEL

LCDBusyLoop();
}

void LCDBusyLoop()
{
	//This function waits till lcd is BUSY

	uint8_t busy,status=0x00,temp;

	//Change Port to input type because we are reading data
	LCD_DATA_DDR&=(~(0x0f<<LCD_DATA_POS));

	//change LCD mode
	SET_RW();		//Read mode
	CLEAR_RS();		//Read status

	//Let the RW/RS lines stabilize

	_delay_us(0.5);		//tAS

	
	do
	{

		SET_E();

		//Wait tDA for data to become available
		_delay_us(0.5);

		status=(LCD_DATA_PIN>>LCD_DATA_POS);
		status=status<<4;

		_delay_us(0.5);

		//Pull E low
		CLEAR_E();
		_delay_us(1);	//tEL

		SET_E();
		_delay_us(0.5);

		temp=(LCD_DATA_PIN>>LCD_DATA_POS);
		temp&=0x0F;

		status=status|temp;

		busy=status & 0b10000000;

		_delay_us(0.5);
		CLEAR_E();
		_delay_us(1);	//tEL
	}while(busy);

	CLEAR_RW();		//write mode
	//Change Port to output
	LCD_DATA_DDR|=(0x0F<<LCD_DATA_POS);

}

void LCDInit(uint8_t style)
{
	/*****************************************************************
	
	This function Initializes the lcd module
	must be called before calling lcd related functions

	Arguments:
	style = LS_BLINK,LS_ULINE(can be "OR"ed for combination)
	LS_BLINK :The cursor is blinking type
	LS_ULINE :Cursor is "underline" type else "block" type

	*****************************************************************/
	
	//After power on Wait for LCD to Initialize
	_delay_ms(30);
	
	//Set IO Ports
	LCD_DATA_DDR|=(0x0F<<LCD_DATA_POS);
	LCD_E_DDR|=(1<<LCD_E_POS);
	LCD_RS_DDR|=(1<<LCD_RS_POS);
	LCD_RW_DDR|=(1<<LCD_RW_POS);

	LCD_DATA_PORT&=(~(0x0F<<LCD_DATA_POS));
	CLEAR_E();
	CLEAR_RW();
	CLEAR_RS();

	//Set 4-bit mode
	_delay_us(0.3);	//tAS

	SET_E();
	LCD_DATA_PORT|=((0b00000010)<<LCD_DATA_POS); //[B] To transfer 0b00100000 i was using LCD_DATA_PORT|=0b00100000
	_delay_us(1);
	CLEAR_E();
	_delay_us(1);
	
	//Wait for LCD to execute the Functionset Command
	LCDBusyLoop();                                    //[B] Forgot this delay

	//Now the LCD is in 4-bit mode

	LCDCmd(0b00101000);			//function set 4-bit,2 line 5x7 dot format
	LCDCmd(0b00001100|style);	//Display On
	
}
void LCDWriteString(const char *msg)
{
	/*****************************************************************
	
	This function Writes a given string to lcd at the current cursor
	location.

	Arguments:
	msg: a null terminated string to print


	*****************************************************************/
 while(*msg!='\0')
 {
	LCDData(*msg);
	msg++;
 }
}

void LCDWriteInt(int val,unsigned int field_length)
{
	/***************************************************************
	This function writes a integer type value to LCD module

	Arguments:
	1)int val	: Value to print

	2)unsigned int field_length :total length of field in which the value is printed
	must be between 1-5 if it is -1 the field length is no of digits in the val

	****************************************************************/

	char str[5]={0,0,0,0,0};
	int i=4,j=0;
	while(val)
	{
	str[i]=val%10;
	val=val/10;
	i--;
	}
	if(field_length==-1)
		while(str[j]==0) j++;
	else
		j=5-field_length;

	if(val<0) LCDData('-');
	for(i=j;i<5;i++)
	{
	LCDData(48+str[i]);
	}
}
void LCDGotoXY(uint8_t x,uint8_t y)
{
	if(x>=20) return;

	#if (LCD_TYPE == LCD204 || LCD_TYPE == LCD162 || LCD_TYPE == LCD202)

	switch(y)
	{
		case 0:
			break;
		case 1:
			x|=0b01000000;
			break;
		case 2:
			x+=0x14;
			break;
		case 3:
			x+=0x54;
			break;
	}

	#elif LCD_TYPE == LCD164

	switch(y)
	{
		case 0:
			break;
		case 1:
			x|=0b01000000;
			break;
		case 2:
			x+=0x10;
			break;
		case 3:
			x+=0x50;
			break;
	}

	#endif

	x|=0b10000000;
  	LCDCmd(x);
}





