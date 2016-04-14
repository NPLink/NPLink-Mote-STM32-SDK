/*
(C)2015 NPLink

Description: OLED driver implementation, used for display.

License: Revised BSD License, see LICENSE.TXT file include in the project

*/

#ifndef __OLED_BOARD_H
#define __OLED_BOARD_H	

#include "stdlib.h"	    
#include "stm32l0xx_hal.h"	    			
		      
#define OLED_CS_Set()     HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_SET)
#define OLED_CS_Reset()   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_RESET)

#define OLED_RST_Set()    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_SET)	
#define OLED_RST_Reset()  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET)	

#define OLED_RS_Set()     HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_SET)
#define OLED_RS_Reset()   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_RESET)

#define OLED_SCLK_Set()   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_SET)
#define OLED_SCLK_Reset() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_RESET)	

#define OLED_SDIN_Set()   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_SET)
#define OLED_SDIN_Reset() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_RESET)	

		     
#define OLED_CMD  0	//Write command
#define OLED_DATA 1	//Write data


/**********Function declaration*************/

void OLED_WR_Byte(u8 dat,u8 cmd);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);  		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_Clear_Half(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size);	 
void oled_test( void );

#endif  //__OLED_BOARD_H
	 







 

