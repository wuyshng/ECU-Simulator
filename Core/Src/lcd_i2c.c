/*
 * lcd_i2c.c
 *
 *  Created on: Mar 17, 2024
 *      Author: quyma
 */


#include "lcd_i2c.h"
#include "tim.h"
#include "obd.h"
#include "string.h"

int32_t curEncPos = 0;
int32_t oldEncPos = 0;
uint32_t stateMenu = 0;

int32_t menuMainCnt = 0;
int32_t menuMainLength = 4;
int32_t optionSelect = 0;
int32_t optionMenuLength = 6;

char *menu[] = {"PIDs Support", "Show Parameters", "Mode 3", "Mode 4"};
char *menuMode1[] = {"Engine Cool Temp", "Engine Speed", "Vehicle Speed", "MAF Sensor", "Throttle", "O2 Voltage"};
char *menuMode2[] = {"CoolTemp", "EngSpd", "VehSpd", "MAFSensor", "Throttle", "O2Volt"};


void LCD_Send_Cmd(char cmd)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (cmd & 0xf0);
	data_l = ((cmd << 4) & 0xf0);
	data_t[0] = data_u | 0x0C ; // en=1, rs=0
	data_t[1] = data_u | 0x08; 	// en=0, rs=0
	data_t[2] = data_l | 0x0C; 	// en=1, rs=0
	data_t[3] = data_l | 0x08; 	// en=0, rs=0
	HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_LCD, (uint8_t *)data_t, 4, 100);
}

void LCD_Send_Data(char data)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data & 0xf0);
	data_l = ((data << 4) & 0xf0);
	data_t[0] = data_u | 0x0D; // en=1, rs=0
	data_t[1] = data_u | 0x09; // en=0, rs=0
	data_t[2] = data_l | 0x0D; // en=1, rs=0
	data_t[3] = data_l | 0x09; // en=0, rs=0
	HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_LCD, (uint8_t *)data_t, 4, 100);
}

void LCD_Init(void)
{
	LCD_Send_Cmd(0x33); /* set 4-bits interface */
	LCD_Send_Cmd(0x32);
	HAL_Delay(50);
	LCD_Send_Cmd(0x28); /* start to set LCD function */
	HAL_Delay(50);
	LCD_Send_Cmd(0x01); /* clear display */
	HAL_Delay(50);
	LCD_Send_Cmd(0x06); /* set entry mode */
	HAL_Delay(50);
	LCD_Send_Cmd(0x0c); /* set display to on */
	HAL_Delay(50);
	LCD_Send_Cmd(0x02); /* move cursor to home and set data address to 0 */
	HAL_Delay(50);
	LCD_Send_Cmd(0x80);
}

void LCD_Send_String(char *str)
{
	while (*str)
		LCD_Send_Data(*str++);
}

void LCD_Clear_Display(void)
{
	LCD_Send_Cmd(0x01); // clear display
}

void LCD_Goto_XY(int x, int y)
{
	if(x == 0)
	{
		LCD_Send_Cmd(0x80 + y);
	}
	else if(x == 1)
	{
		LCD_Send_Cmd(0xC0 + y);
	}
}

void LCD_Clear_Line(int line)
{
	LCD_Goto_XY(line, 0);
	for(int n = 0; n < 16; n++)
	{
		LCD_Send_String(" ");
	}
}

void loop_menu(void)
{
	curEncPos = rotary_encoder_value();
	if(oldEncPos != curEncPos)
	{
		if(oldEncPos < curEncPos) {
			HAL_Delay(500);
			menuMainCnt++;
		}
		else {
			HAL_Delay(500);
			menuMainCnt--;
		}
		if(menuMainCnt >= menuMainLength) {
			menuMainCnt = 0;
		}
		else if(menuMainCnt < 0) {
			menuMainCnt = menuMainLength-1;
		}
		oldEncPos = curEncPos;
		update_menu(menuMainCnt);
		//HAL_Delay(100);
	}
	/* Using pushbutton to select Menu */
	if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == 0)
	{
		while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == 0);
		stateMenu = 1;

	}
}

void update_menu(int menuSelect)
{
	uint32_t select = 1;
	if (menuSelect % 2 == 0) {
		select = 2;
		if (menuSelect >= menuMainLength - 1) {
			select = 0;
		}
	}
	switch (select)
	{
		case 2:
			LCD_Clear_Display();
			HAL_Delay(50);
			LCD_Goto_XY(0, 0);
			LCD_Send_String(">");
			LCD_Goto_XY(0, 1);
			LCD_Send_String(menu[menuSelect]);
			LCD_Goto_XY(1, 1);
			LCD_Send_String(menu[menuSelect + 1]);
			HAL_Delay(50);
			break;
		case 1:
			LCD_Clear_Display();
			HAL_Delay(50);
			LCD_Goto_XY(0, 1);
			LCD_Send_String(menu[menuSelect - 1]);
			LCD_Goto_XY(1, 0);
			LCD_Send_String(">");
			LCD_Goto_XY(1, 1);
			LCD_Send_String(menu[menuSelect]);
			HAL_Delay(50);
			break;
		case 0:
			LCD_Clear_Display();
			HAL_Delay(50);
			LCD_Goto_XY(0, 0);
			LCD_Send_String(">");
			LCD_Goto_XY(0, 1);
			LCD_Send_String(menu[menuSelect]);
			HAL_Delay(50);
			break;
	}
}

void setting_value(int menuSelect)
{
	update_setting_value(menuSelect);
	curEncPos = rotary_encoder_value();
	if (oldEncPos != curEncPos)
	{
		if (oldEncPos < curEncPos) {
			HAL_Delay(500);
			optionSelect++;
		}
		else {
			HAL_Delay(500);
			optionSelect--;
		}
		if(optionSelect >= optionMenuLength) {
			optionSelect = 0;
		}
		else if(optionSelect < 0) {
			optionSelect = optionMenuLength - 1;
		}
		oldEncPos = curEncPos;
		update_setting_value(menuSelect);
	}
	/* Using pushbutton to back main menu */
	if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == 0) {
		while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == 0);
		optionSelect = 0;
		stateMenu = 0;
		update_menu(menuMainCnt);
	}
}

void update_setting_value(int menuSelect)
{
	LCD_Clear_Display();
	HAL_Delay(10);
	LCD_Goto_XY(0, 0);
	LCD_Send_String(">Back");
	LCD_Goto_XY(1, 0);
	switch (menuSelect)
	{
		case 0:
			LCD_Send_String(menuMode1[optionSelect]);
			//HAL_Delay(10);
			break;
		case 1:
//			LCD_Clear_Display();
//			HAL_Delay(42);
			LCD_Clear_Line(1);
//			LCD_Goto_XY(0, 0);
//			LCD_Send_String(">Back");
			LCD_Goto_XY(1, 0);
			LCD_Send_String(menuMode2[optionSelect]);
			uint8_t posValuePID = strlen(menuMode2[optionSelect]);
			LCD_Goto_XY(1, posValuePID + 1);
			display_ecu_value(optionSelect);
			HAL_Delay(50);
			break;
	}
}
