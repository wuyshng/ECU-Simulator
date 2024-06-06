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

uint8_t stateMenu = 0;
uint8_t oldStateMenu = 0;
int32_t menuMainCnt = 0;
int32_t menuMainLength = 3;
int32_t optionSelect = 0;
int32_t optionMenuLength = 6;
int8_t mode3Length = 2;
int8_t mode3Select = 0;
uint8_t oldStateECUDTC = 0;

extern float curPIDValue;
extern float oldPIDValue;

char *menu[] = {"PIDs SUPPORT", "SHOW CUR DATA", "SHOW STORED DTC"};
char *menuMode1[] = {"Engine Cool Temp", "Engine Speed", "Vehicle Speed", "MAF Sensor", "Throttle", "O2 Voltage"};
char *menuMode2[] = {"CoolTemp", "EngSpd", "VehSpd", "MAFSensor", "Throttle", "O2Volt"};
char *menuMode3[] = {"P0101", "U0158"};

void LCD_Send_Cmd(char cmd)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (cmd & 0xf0);
	data_l = ((cmd << 4) & 0xf0);
	data_t[0] = data_u | 0x0C; // en=1, rs=0
	data_t[1] = data_u | 0x08; // en=0, rs=0
	data_t[2] = data_l | 0x0C; // en=1, rs=0
	data_t[3] = data_l | 0x08; // en=0, rs=0
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
	LCD_Send_Cmd(0x01);
}

void LCD_Goto_XY(int x, int y)
{
	if (x == 0)
	{
		LCD_Send_Cmd(0x80 + y);
	}
	else if (x == 1)
	{
		LCD_Send_Cmd(0xC0 + y);
	}
}

void LCD_Clear_Line(int rowNum)
{
	LCD_Goto_XY(rowNum, 0);
	for (int i = 0; i < 16; i++)
	{
		LCD_Send_String(" ");
	}
	LCD_Goto_XY(rowNum, 0);
}

void LCD_Clear_Pos(int rowNum, int colNum)
{
	LCD_Goto_XY(rowNum, colNum);
	for (int i = colNum; i < 16; i++)
	{
		LCD_Send_String(" ");
	}
	HAL_Delay(10);
	LCD_Goto_XY(rowNum, colNum);
}

void push_button_menu(void)
{
	if (stateMenu != oldStateMenu)
	{
		if (stateMenu == 0)
		{
			update_menu(menuMainCnt);
		}
		oldStateMenu = stateMenu;
	}
}

void loop_menu(void)
{
	curEncPos = rotary_encoder_value();
	if (oldEncPos != curEncPos)
	{
		if (oldEncPos < curEncPos)
		{
			HAL_Delay(1000);
			menuMainCnt++;
		}
		else
		{
			HAL_Delay(1000);
			menuMainCnt--;
		}

		if (menuMainCnt >= menuMainLength)
		{
			menuMainCnt = 0;
		}
		else if (menuMainCnt < 0)
		{
			menuMainCnt = menuMainLength - 1;
		}
		oldEncPos = curEncPos;
		update_menu(menuMainCnt);
	}
	/* Using pushbutton to select Menu */
	if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == 0)
	{
		while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == 0);
		stateMenu = 1;
		LCD_Clear_Display();
		HAL_Delay(10);
	}
}

void update_menu(int menuSelect)
{
	uint32_t select = 1;
	if (menuSelect % 2 == 0)
	{
		select = 2;
		if (menuSelect >= menuMainLength - 1)
		{
			select = 0;
		}
	}
	switch (select)
	{
	case 2:
		LCD_Clear_Display();
		HAL_Delay(25);
		LCD_Goto_XY(0, 0);
		LCD_Send_String(">");
		LCD_Goto_XY(0, 1);
		LCD_Send_String(menu[menuSelect]);
		LCD_Goto_XY(1, 1);
		LCD_Send_String(menu[menuSelect + 1]);
		HAL_Delay(25);
		break;
	case 1:
		LCD_Clear_Display();
		HAL_Delay(25);
		LCD_Goto_XY(0, 1);
		LCD_Send_String(menu[menuSelect - 1]);
		LCD_Goto_XY(1, 0);
		LCD_Send_String(">");
		LCD_Goto_XY(1, 1);
		LCD_Send_String(menu[menuSelect]);
		HAL_Delay(25);
		break;
	case 0:
		LCD_Clear_Display();
		HAL_Delay(25);
		LCD_Goto_XY(0, 0);
		LCD_Send_String(">");
		LCD_Goto_XY(0, 1);
		LCD_Send_String(menu[menuSelect]);
		HAL_Delay(25);
		break;
	}
}

void setting_value(int menuSelect)
{
	update_setting_value(menuSelect);
	if (menuMainCnt == 2)
	{
		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2) == 0)
		{
			LCD_Clear_Line(1);
			HAL_Delay(10);
		}
	}

	curEncPos = rotary_encoder_value();
	if (oldEncPos != curEncPos)
	{
		if (oldEncPos < curEncPos)
		{
			HAL_Delay(1000);
			optionSelect++;
			mode3Select++;
		}
		else
		{
			HAL_Delay(1000);
			optionSelect--;
			mode3Select--;
		}

		if (optionSelect >= optionMenuLength)
		{
			optionSelect = 0;
		}
		else if (optionSelect < 0)
		{
			optionSelect = optionMenuLength - 1;
		}

		if (mode3Select >= mode3Length)
		{
			mode3Select = 0;
		}
		else if (mode3Select < 0)
		{
			mode3Select = mode3Length - 1;
		}
		oldEncPos = curEncPos;
		LCD_Clear_Line(1);
		HAL_Delay(10);
		update_setting_value(menuSelect);
	}
	/* Using pushbutton to back main menu */
	if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == 0)
	{
		while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == 0);
		optionSelect = 0;
		stateMenu = 0;
		update_menu(menuMainCnt);
	}
}

void update_setting_value(int menuSelect)
{
	LCD_Goto_XY(0, 0);
	LCD_Send_String(">Back");
	LCD_Goto_XY(1, 0);
	switch (menuSelect)
	{
	case 0:
		LCD_Send_String(menuMode1[optionSelect]);
		break;
	case 1:
		ecu_display_LCD(optionSelect);
		if (curPIDValue != oldPIDValue)
		{
			LCD_Send_String(menuMode2[optionSelect]);
			uint8_t posValuePID = strlen(menuMode2[optionSelect]);
			LCD_Clear_Pos(1, posValuePID + 1);
			display_ecu_value(optionSelect);
			HAL_Delay(10);
		}
		oldPIDValue = curPIDValue;
		break;
	case 2:
		if (ecuDTC == 0)
		{
			LCD_Send_String("No stored DTC");
			break;
		}
		else
		{
			LCD_Send_String(menuMode3[mode3Select]);
		}
		break;
	}
}
