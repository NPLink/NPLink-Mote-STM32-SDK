/*
(C)2015 NPLink

Description: OLED driver implementation, used for display.

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Robxr
*/

#include "oled_board.h"
#include "stdlib.h"
#include "oledfont.h"  	 


//OLED Buffer
//the format is as follows:
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 		   
u8 OLED_GRAM[128][8];	 

/*********************************************************************
 * @fn		OLED_Refresh_Gram
 *
 * @brief 	 update oled  display.
 *
 * @param 	void
 *
 * @return	void
 */
void OLED_Refresh_Gram(void)
{
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //SET ADDRESS(0-7)
		OLED_WR_Byte (0x00,OLED_CMD);      // SET THE DISPLAY POSITION  - LOW BYTE  
		OLED_WR_Byte (0x10,OLED_CMD);      // SET THE DISPLAY POSITION  - HIGH BYTE
		for(n=0;n<128;n++)
			OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA); 
	}   
}

/*********************************************************************
 * @fn		OLED_Refresh_Gram
 *
 * @brief 	 write a byte to SSD1306 
 *
 * @param 	u8  data/commond
 *
 * @param 	u8 cmd:    0 - commond    1 - data
 *
 * @return	void
 */
void OLED_WR_Byte(u8 dat,u8 cmd)
{	
		u8 i;			  
		if(1 == cmd)
		{
				OLED_RS_Set();
		}
		else
		{
				OLED_RS_Reset();
		}
	
			OLED_CS_Reset();	
	
		for(i = 0;i < 8;i++)
		{			  
			OLED_SCLK_Reset();
			
			if(dat & 0x80)
			{
					OLED_SDIN_Set();
			}
			else
			{
					OLED_SDIN_Reset();
			}
			
				OLED_SCLK_Set();
				dat <<= 1;   
		}
	
		OLED_CS_Set();		  
		OLED_RS_Set();   	  
} 
	  
/*********************************************************************
 * @fn		OLED_Display_On
 *
 * @brief 	
 *
 * @param 	void
 *
 * @return	void
 */
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC COMMOND
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}

/*********************************************************************
 * @fn		OLED_Display_Off
 *
 * @brief 	
 *
 * @param 	void
 *
 * @return	void
 */
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC COMMOND
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}	

/*********************************************************************
 * @fn		OLED_Clear
 *
 * @brief 	clear the LED display, all the screen will be black.
 *
 * @param 	void
 *
 * @return	void
 */	  
void OLED_Clear(void)  
{  
	u8 i,n;  
	for(i=0;i<8;i++)for(n=0;n<128;n++)
		OLED_GRAM[n][i]=0X00;  
		OLED_Refresh_Gram();
}

void OLED_Clear_char(void)  
{  
	u8 i,n;  
	for(i=0;i<1;i++)for(n=70;n<128;n++)
		OLED_GRAM[n][i]=0X00;  
		OLED_Refresh_Gram();
}


/*********************************************************************
 * @fn		OLED_Fill_All
 *
 * @brief 	fill the LED display, all the screen will be black.
 *
 * @param 	void
 *
 * @return	void
 */	
void	OLED_Fill_All(void)
{  
	u8 i,n;  
	for(i=0;i<8;i++)for(n=0;n<128;n++)
		OLED_GRAM[n][i]=0XFF;  
		OLED_Refresh_Gram();
}

/*********************************************************************
 * @fn		OLED_Clear_line
 *
 * @brief 	clear the LED display, all the screen will be black.
 *
 * @param 	lineNumber <= 64/size,size = 12,16,24
 *
 * @return	void
 */	

void OLED_Clear_Line(u8 lineNumber,u8 size)  
{  
	u8 n,line;  
  
	if((size != 12) && (size != 16) && (size != 24))
		return;
	if(lineNumber < 1 || lineNumber >( 64/size))
			return ;
	
	line = 64/size+1 - lineNumber;

	if(size == 12)
	{
//		line = 6 - line;
	  if(line % 2)
		{
			for(n=0;n<128;n++)
			{
				OLED_GRAM[n][(line/2)*3 +1] = 0X00;

				OLED_GRAM[n][(line/2)*3] &= 0X0F;
			}
		}
		else
		{
			for(n=0;n<128;n++)
			{
				OLED_GRAM[n][(line/2)*3] &= 0Xf0;

				OLED_GRAM[n][(line/2)*3 - 1] = 0X00;
			}
		}
	}
	
	if(size == 16)
	{
//		line = 5 - line;
		for(n=0;n<128;n++)
		{
			OLED_GRAM[n][2*line-1] = 0X00;

			OLED_GRAM[n][2*line-2] = 0X00;
		}
	}
	
	if(size == 24)
	{
//		line = 3 - line;
		for(n=0;n<128;n++)
		{
			OLED_GRAM[n][3*line+1] = 0X00;

			OLED_GRAM[n][3*line] = 0X00;
			
			OLED_GRAM[n][3*line-1] = 0X00;
		}
	}
	OLED_Refresh_Gram();
}

/*********************************************************************
 * @fn		OLED_Clear_Half
 *
 * @brief 	fill the screen with black but not update display , need external update
 *
 * @param 	void
 *
 * @return	void
 */	  
void OLED_Clear_Half(void)  
{  
	u8 i,n;  
	for(i=0;i<2;i++)
		for(n=0;n<128;n++)
		    OLED_GRAM[n][i]=0X00;  
	OLED_Refresh_Gram();
}

/*********************************************************************
 * @fn		OLED_DrawPoint
 *
 * @brief 	draw point
 *
 * @param 	u8 x    x Coordinate
 *
  * @param 	u8 y    y Coordinate
  *
 * @return	u8 t    1 - fill    0 - clean
 */	
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;  //  Out of range
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)
		OLED_GRAM[x][pos]|=temp;
	else 
		OLED_GRAM[x][pos]&=~temp;	    
}

/*******************************************************************************************
 * @fn		OLED_Fill
 *
 * @brief 	draw  Diagonal coordinates point of  the filling region ,make sure x1<=x2,y1<=y2,0<=x1<=127,0<=y1<=63
 *
 * @param 	u8 x1    Diagonal coordinates
 *a
 * @param 	u8 y1    Diagonal coordinates
 *
 * @param 	u8 x2    Diagonal coordinates
 *
 * @param 	u8 y2    Diagonal coordinates
 *
 * @param  	u8 dot    1 - fill    0 - clean
 *
 * @return	void
 */
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
{  
	u8 x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot);
	}													    
	OLED_Refresh_Gram();//update display
}

/**************************************************
 * @fn		OLED_Fill
 *
 * @brief 	displaying a word or some character at the Designated position 
 *
 * @param 	u8 x      0 ~ 127
 *
 * @param 	u8 y      0 ~ 63
 *
 * @param 	u8 chr    
 *
 * @param 	u8 size     
 *
 * @param   u8 mode   1 - normal display    0 - The white display
 *
 * @return   void
 */

void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		
	 chr=chr-' ';                                 //Get the migration result
    for(t=0;t<csize;t++)
    {   
			if(size==12)temp=asc2_1206[chr][t]; 	 	  // Font 1206
			else if(size==16)temp=asc2_1608[chr][t];	// Font 1206
			else if(size==24)temp=asc2_2412[chr][t];	// Font 1206
			else return;								              //Not word stock
      for(t1=0;t1<8;t1++)
			{
					if(temp&0x80)OLED_DrawPoint(x,y,mode);
					else OLED_DrawPoint(x,y,!mode);
					temp<<=1;
					y++;
					if((y-y0)==size)
					{
							y=y0;
							x++;
							break;
					}
			}  	 
    }          
}

/**************************************************
 * @fn		mypow
 *
 * @brief 	
 *
 * @param 	u8 m      
 *
 * @param 	u8 n     
 *
 * @return	return a type of u32 data result
 */
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				 

/**************************************************
 * @fn		OLED_ShowNum
 *
 * @brief 	display two numbers
 *
 * @param 	u8 x   The start coordinates  
 *
 * @param 	u8 y   The start coordinates 
 *
 * @param 	u32 num   0~4294907295
 *
 * @param 	u8 len   the number of digits
 *
 * @param 	u8 size   the size of string
 *
 * @return	void
 */ 		  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',size,1);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 
	}
} 
 
/**************************************************
 * @fn		OLED_ShowString
 *
 * @brief 	display string
 *
 * @param 	u8 x   The start coordinates  
 *
 * @param 	u8 y   The start coordinates 
 *
 * @param 	u8 *p    Point to the string
 *
 * @param 	u8 size   the size of string
 *
 * @return	void
 */
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size)
{	
    while((*p<='~')&&(*p>=' '))   //Determine whether illegal
    {       
        if(x>(128-(size/2))){x=0;y+=size;}
        if(y>(64-size)){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,size,1);	 
        x+=size/2;
        p++;
    }  
	
}	   

/**************************************************
 * @fn		OLED_Init
 *
 * @brief 	Initialize SSD1306    
 *
 * @return	void
 */
void OLED_Init(void)
{ 	
 
   GPIO_InitTypeDef  GPIO_InitStruct;
 
  __GPIOA_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_MEDIUM;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_SET);

	GPIO_InitStruct.Pin = GPIO_PIN_12| GPIO_PIN_13| GPIO_PIN_14| GPIO_PIN_15;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); 
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12| GPIO_PIN_13| GPIO_PIN_14| GPIO_PIN_15,GPIO_PIN_SET);
	OLED_CS_Set();
	OLED_RS_Set();	 
	OLED_RST_Reset();
	HAL_Delay(100);
	OLED_RST_Set(); 
					  
	OLED_WR_Byte(0xAE,OLED_CMD); //Close display
	OLED_WR_Byte(0xD5,OLED_CMD); //Set clock crossover factor and Oscillation frequency
	OLED_WR_Byte(80,OLED_CMD);   //Crossover factor - [3:0]; Oscillation frequency - [7:4]
	OLED_WR_Byte(0xA8,OLED_CMD); //set the number of drive
	OLED_WR_Byte(0X3F,OLED_CMD); //0X3F(1/64)  defalt
	OLED_WR_Byte(0xD3,OLED_CMD); //set the offset of display
	OLED_WR_Byte(0X00,OLED_CMD); // 0 default

	OLED_WR_Byte(0x40,OLED_CMD); //set start with display  [5:0] rows
													    
	OLED_WR_Byte(0x8D,OLED_CMD); //µÁ∫…±√…Ë÷√
	OLED_WR_Byte(0x14,OLED_CMD); //bit2£¨open/close
	OLED_WR_Byte(0x20,OLED_CMD); //set memory address mode
	OLED_WR_Byte(0x02,OLED_CMD); //[1:0],00£¨¡–µÿ÷∑ƒ£ Ω;01£¨––µÿ÷∑ƒ£ Ω;10,“≥µÿ÷∑ƒ£ Ω;ƒ¨»œ10;
	OLED_WR_Byte(0xA1,OLED_CMD); //∂Œ÷ÿ∂®“Â…Ë÷√,bit0:0,0->0;1,0->127;
	OLED_WR_Byte(0xC0,OLED_CMD); //…Ë÷√COM…®√Ë∑ΩœÚ;bit3:0,∆’Õ®ƒ£ Ω;1,÷ÿ∂®“Âƒ£ Ω COM[N-1]->COM0;N:«˝∂Ø¬∑ ˝
	OLED_WR_Byte(0xDA,OLED_CMD); //…Ë÷√COM”≤º˛“˝Ω≈≈‰÷√
	OLED_WR_Byte(0x12,OLED_CMD); //[5:4]config
		 
	OLED_WR_Byte(0x81,OLED_CMD); //∂‘±»∂»…Ë÷√
	OLED_WR_Byte(0xEF,OLED_CMD); //1~255;ƒ¨»œ0X7F (¡¡∂»…Ë÷√,‘Ω¥Û‘Ω¡¡)
	OLED_WR_Byte(0xD9,OLED_CMD); //…Ë÷√‘§≥‰µÁ÷‹∆⁄
	OLED_WR_Byte(0xf1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
	OLED_WR_Byte(0xDB,OLED_CMD); //…Ë÷√VCOMH µÁ—π±∂¬ 
	OLED_WR_Byte(0x30,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	OLED_WR_Byte(0xA4,OLED_CMD); //»´æ÷œ‘ æø™∆Ù;bit0:1,ø™∆Ù;0,πÿ±’;(∞◊∆¡/∫⁄∆¡)
	OLED_WR_Byte(0xA6,OLED_CMD); //…œ‘ æ∑Ω Ω;bit0:1,∑¥œ‡œ‘ æ;0,’˝≥£œ‘ æ	  set display mode  0-Normal display   						   
	OLED_WR_Byte(0xAF,OLED_CMD); //open display	 
	OLED_Clear();
}  

/**************************************************
 * @fn		oled_test
 *
 * @brief 	    
 *
 * @return	void
 */
void oled_test( void )
{
	OLED_ShowString(0,0,"ALIENTEK",24);  
	OLED_ShowString(0,24, "0.96' OLED TEST",16);  
	OLED_ShowString(0,40,"ATOM 2015/1/14",12);  
	OLED_ShowString(0,52,"ASCII:",12);  
	OLED_ShowString(64,52,"CODE:",12);  
	OLED_Refresh_Gram();		
}


























