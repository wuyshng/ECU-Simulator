/*
 * lcd_i2c.h
 *
 *  Created on: Mar 17, 2024
 *      Author: quyma
 */

#ifndef INC_LCD_I2C_H_
#define INC_LCD_I2C_H_

#include "main.h"

#define SLAVE_ADDRESS_LCD 0x27 << 1
extern I2C_HandleTypeDef hi2c1;

extern int32_t curEncPos;
extern int32_t oldEncPos;
extern uint8_t stateMenu;
extern uint8_t oldStateMenu;
extern int32_t menuMainCnt;
extern int32_t menuMainLength;
extern int32_t optionSelect;
extern int32_t optionMenuLength;

void LCD_Init(void);					        // initialize lcd
void LCD_Send_Cmd(char cmd);			        // send command to the lcd
void LCD_Send_Data(char data);			        // send data to the lcd
void LCD_Send_String(char *str);		        // send string to the lcd
void LCD_Clear_Display(void);			        // clear display lcd
void LCD_Goto_XY(int row, int col);		        // set proper location on screen
void LCD_Clear_Line(int rowNum);				// clear line
void LCD_Clear_Pos(int rowNum, int colNum);		// clear at position
void loop_menu(void);
void update_menu(int menuSelect);
void setting_value(int menuSelect);
void update_setting_value(int menuSelect);
void push_button_menu(void);

#endif /* INC_LCD_I2C_H_ */
