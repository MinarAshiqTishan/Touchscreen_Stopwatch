//Touch and counting starts. touch again to stop counting ; 
// press button to reset;
//perfect time interval ;
//lap time storage;

#include <avr/io.h>
#include <util/delay.h>
#include "lcd_lib.h"
#include <avr/pgmspace.h>
#include <stdio.h>
#include<math.h>
#include <avr/interrupt.h>
#include<string.h>

/*LCD functions are - 
1. 	LCDsendChar()			-	writes character on LCD
2. 	LCDsendCommand()		-	Sends command to LCD
3. 	LCDinit(void)			-	Initializes LCD
4.	LCDclr(void)			-	Clears LCD
5.	LCDhome(void)			-	LCD cursor home
6.	LCDstring(*data, bytes)	-	Writes string to LCD
7.	LCDGotoXY(X,Y)			-	Cursor to X, Y position
8.	CopyStringtoLCD()		-	Copy string from flash memory to LCD
9.	LCDdefinechar()			-	Define Character symbol in CGRAM
10.	LCDshiftLeft(n)			-	Shifts character on LCD Left	
11.	LCDshiftRight(n)		-	Shifts character on LCD Right
12.	LCDcursorOn(void)		-	Cursor is visible
13.	LCDcursorOnBlink(void)	-	Cursor is on and blinking
14.	LCDcursorOFF(void)		-	Cursor is invisible
15.	LCDblank(void)			-	Blanks the LCD
16.	LCDvisible(void)		-	Shows LCD
17.	LCDcursorLeft(n)		-	Shifts the cursor left by n place
18.	LCDcursorRight(n)		-	Shifts the cursor right by n place
19.	LCDprogressBar()		-	progress bar on LCD

*/
volatile uint8_t flag=0, flag2=0,stc=0;
volatile char str_yu[]={"00:00:00:00"}, store[9][15]={},strc[15];

void (*resetptr)( void ) = 0x0000;  //start program from the start
									//not used

/*void init_int1(void)
{
	sei();

	MCUCR|=(1<<ISC01);
	GICR|=(1<<INT1);

}
*/

int showtime(int time,int x,int y,int strpos1,int strpos2)
{	
	LCDGotoXY(x,y);
	str_yu[strpos1] = ((time) % 10) + 48;
	str_yu[strpos2] = (((time) / 10) % 10) + 48;	
				 
	LCDsendChar(str_yu[strpos2]);
	LCDsendChar(str_yu[strpos1]);

 return 0;
}



int main(void)
{
	int sec=0,min=0,hour=0, micro = 0,k=0,j=0;
	char str[]={"00:00:00:00"},yo[9][3]={"1.","2.","3.","4.","5.","6.","7.","8.","9."};
	char str2[]={"hh:mm:ss:ms"};
    char credit[]={"    STOPWATCH"};
    char credit2[]={"Tishan 101013"};
	int p=0;	
	
	DDRA=0x00;
	DDRB=0b11111111;
	PORTB=0;

	LCDinit();	
	LCDGotoXY(2,0);
	LCDstring(&str,11);
	LCDGotoXY(2,1);
	LCDstring(&str2,11);
	//sei();
	//init_int1();
	
	// Prescaler = FCPU/1024
	//TCCR0|=(1<<CS01);

	//Enable Overflow Interrupt Enable
	//TIMSK|=(1<<TOIE0);
	
	while(1)
	{
	
	//DDRB=0b01100000;
	//DDRA=0x0c;
	//PORTB=0b00100000; 
	
	
	 
	
	 if(flag==0)
	 {	    if((PINB & 1)){goto RESET;}
	 		while(!(PINA & 1))
			{
				k+=1;
				j++;
				_delay_ms(10);
			}
			
			if(k<=20 && k>0) 
			{
				 micro++;
				 j=0;
				 k=20;
				
			}
			else if(j>100){ 
							goto LAPS;
							
							RESET:
							// display credit
							LCDclr();
							LCDGotoXY(0,0);
							LCDstring(&credit,13);
							LCDGotoXY(0,1);
							LCDstring(&credit2,13);
							_delay_ms(200);
						   
							
	  						LCDclr();
							
							// reset everything
							sec=0,min=0,hour=0, micro = 0,k=0;
							 
							
							for(j=0;j<13;j++)
								str_yu[j]='0';
							str_yu[2]=':';
							str_yu[5]=':';	
							str_yu[8]=':';
							
							j=0;
							
							//show 00:00:00:00:00
							LCDGotoXY(2,0);
							LCDstring(&str,11);
							LCDGotoXY(2,1);
							LCDstring(&str2,11);
	 						  

						 }
			
			else if(k>20){
							k=0;
							j=0;
							//yo[0]=stc+48;
							//yo[2]='\0';
							
							//for(j=0;j<13;j++)
							//	str_yu[j]='0';
							//str_yu[2]=':';
							//str_yu[5]=':';	
							//str_yu[8]=':';
							//strcpy(strc,str_yu)	;
							//strcat(yo,str_yu);
							_delay_ms(100);
							strcpy(store[stc],str_yu);
							stc+=1;
							if(stc>7)stc=0;

										
						  }
	   // if(!(PINA & 1))
	  
		
		if(micro==100)
		{    
		     sec++;
			 micro=0;
			 
			 if(sec<60)
			 {
				 showtime(sec,8,0,7,6);
				 
	         }
			 
			 if(sec==60)
			 {
				  sec=0;
				  min++;
				  
				  showtime(sec,8,0,7,6);
				  
				  if(min<60)
			      {
					 showtime(min,5,0,4,3);
					 
	              }
				  
				  if(min==60)
				  {
					   min=0;
					   showtime(min,5,0,4,3);	
					   hour++;
					   
					   if(hour<12)
					   {
					     showtime(hour,2,0,1,0);
						 
					   }
						   
					   if(hour==12)
				       {    hour=0;	
					        showtime(hour,2,0,1,0);
							
		               }
			      }
			 }
					  
		}		
				
			
			
			
			if(micro<100)
	        {
				showtime(micro,11,0,10,9);
			
		    }

			
            
			
			
	   	
      }
	  
    }
while(1){	
	LAPS:
		
		LCDGotoXY(0,0);
		LCDstring(&yo[p],2);
		LCDstring(&store[p],11);
	
	    
		LCDGotoXY(0,1);
		LCDstring(&yo[p+1],2);
		LCDstring(&store[p+1],11);
		
		if(!(PINA & 1))p+=2;
		
		if(p==8){p=0;}
		
		_delay_ms(250);
		
		if((PINB & 1)){p=0;break;}
		}
		goto RESET;
		
return 0;	
	
}
	

//ISR(INT1_vect)
//{
//	goto RESET;
//}
/*

ISR(TIMER0_OVF_vect)
{
   int input=PINB;
   
	//char str[]={"00:00:00:00"};
	//+char str2[]={"hh:mm:ss:ms"};	
   
   
   if((input & 1 ))
   {

	
   }
   else
   {
    //LCDblank();
	//flag=0;	
		
   }
}
*/
