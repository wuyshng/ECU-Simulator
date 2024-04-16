/*
 * obd.c
 *
 *  Created on: Mar 4, 2024
 *      Author: quyma
 */

#include "obd.h"
#include "lcd_i2c.h"
#include "tim.h"

OBD2Data obd2_data;
ECUValue ecuValue;

int gen_random_number(void)
{
    return rand() % (4095 + 1);
}

void ecu_scale_analog_value(void)
{
//	adcScanValue[0] = gen_random_number();
//	adcScanValue[1] = gen_random_number();
//	adcScanValue[2] = gen_random_number();
	adcScanValue[3] = gen_random_number();
	adcScanValue[4] = gen_random_number();
	adcScanValue[5] = gen_random_number();

	for(uint8_t i=0; i<hadc1.Init.NbrOfConversion; i++) {
		obd2_data.engine_coolant_temp = map(adcScanValue[0], 0, 4095, 0, 0xFF);
		obd2_data.engine_speed = map(adcScanValue[1], 0, 4095, 0, 0xFFFF);
		obd2_data.vehicle_speed = map(adcScanValue[2], 0, 4095, 0, 0xFF);
		obd2_data.maf_sensor = map(adcScanValue[3], 0, 4095, 0, 0xFFFF);
		obd2_data.throttle = map(adcScanValue[4], 0, 4095, 0, 0xFF);
		obd2_data.o2_voltage = map(adcScanValue[5], 0, 4095, 0, 0xFFFF);
	}
}

void test_value_ecu(void)
{
	ecuValue.engine_coolant_temp = obd2_data.engine_coolant_temp;
	ecuValue.engine_speed = obd2_data.engine_speed;
	ecuValue.vehicle_speed = obd2_data.vehicle_speed;
	ecuValue.maf_sensor = obd2_data.maf_sensor;
	ecuValue.throttle = obd2_data.throttle;
	ecuValue.o2_voltage = obd2_data.o2_voltage;
}

void ecu_send_respond(void)
{
	TxHeader.StdId = PID_REPLY;		/* ID message */
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.DLC = 8;			/* Data length */

	if(RxData[1] == MODE1)
	{
		TxData[1] = MODE1_RESPONSE;
		switch(RxData[2])
		{
			case PID_SUPPORTED:
				TxData[0] = 0x06;
				TxData[2] = PID_SUPPORTED;
				TxData[3] = 0xC8;
				TxData[4] = 0x19;
				TxData[5] = 0xB0;
				TxData[6] = 0x00;
				TxData[7] = 0x00;
				HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox);
				break;

			case MONITOR_STATUS:
				TxData[0] = 0x05;
				TxData[2] = MONITOR_STATUS;
				TxData[3] = 0xE8;
				TxData[4] = 0x07;
				TxData[5] = 0xFF;
				HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox);
				break;

			case ENGINE_COOLANT_TEMP:		//	A-40 [degree C]
				TxData[0] = 0x03;
				TxData[2] = ENGINE_COOLANT_TEMP;
				TxData[3] = obd2_data.engine_coolant_temp;
				HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox);
				ecuValue.engine_coolant_temp = TxData[3] - 40;
				break;

			case ENGINE_SPEED:				//	((A*256)+B)/4 [RPM]
				TxData[0] = 0x04;
				TxData[2] = ENGINE_SPEED;
				TxData[3] = (obd2_data.engine_speed & 0xff00) >> 8;
				TxData[4] = obd2_data.engine_speed & 0x00ff;
				HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox);
				ecuValue.engine_speed = (TxData[3] * 256 + TxData[4]) / 4;
				break;

			case VEHICLE_SPEED:				//	A [km/h]
				TxData[0] = 0x03;
				TxData[2] = VEHICLE_SPEED;
				TxData[3] = obd2_data.vehicle_speed;
				HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox);
				ecuValue.vehicle_speed = TxData[3];
				break;

			case MAF_SENSOR:				//	((256*A)+B)/100 [g/s]
				TxData[0] = 0x04;
				TxData[2] = MAF_SENSOR;
				TxData[3] = (obd2_data.maf_sensor & 0xff00) >> 8;
				TxData[4] =  obd2_data.maf_sensor & 0x00ff;
				HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox);
				ecuValue.maf_sensor = (TxData[3] * 256 + TxData[4]) / 100;
				break;

			case THROTTLE:					// 100*A/255 [%]
				TxData[0] = 0x03;
				TxData[2] = THROTTLE;
				TxData[3] = obd2_data.throttle;
				HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox);
				ecuValue.throttle = TxData[3] * 100 / 255;
				break;

			case O2_VOLTAGE:				//	A*0.005 or (B-128)*100/128 (if B==0xFF, sensor is not used in trim calc)
				TxData[0] = 0x04;
				TxData[2] = O2_VOLTAGE;
				TxData[3] = (obd2_data.o2_voltage & 0xff00) >> 8;
				TxData[4] = obd2_data.o2_voltage & 0x00ff;
				HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox);
				ecuValue.o2_voltage = (100 * TxData[4] / 128) - 128;
				break;
		}
	}

	if(RxData[1] == MODE3)
	{
		TxData[0] = 0x06;
		TxData[1] = MODE3_RESPONSE;
		TxData[2] = 0x02;
		TxData[3] = 0x01;
		TxData[4] = 0x00;
		TxData[5] = 0x00;
		TxData[6] = 0x00;
	}

	if(RxData[1] == MODE4)
	{
		TxData[0] = 0x00;
		TxData[1] = MODE4_RESPONSE;
	}
}

long map(long inVal, long inMin, long inMax, long outMin, long outMax) {
  return ((inVal - inMin) * (outMax - outMin) / (inMax - inMin) + outMin);
}

void display_ecu_value(int32_t PIDValue)
{
	//test_value_ecu();
	ecu_send_respond();
    char ecu_buf[17];
    switch(PIDValue)
    {
    	case 0:
    		sprintf(ecu_buf, "%doC", ecuValue.engine_coolant_temp);
			LCD_Send_String(ecu_buf);
			break;
    	case 1:
    		sprintf(ecu_buf, "%dRPM", ecuValue.engine_speed);
			LCD_Send_String(ecu_buf);
			break;
    	case 2:
    		sprintf(ecu_buf, "%dkm/h", ecuValue.vehicle_speed);
			LCD_Send_String(ecu_buf);
			break;
    	case 3:
    		sprintf(ecu_buf, "%dg/s", ecuValue.maf_sensor);
			LCD_Send_String(ecu_buf);
			break;
    	case 4:
    		sprintf(ecu_buf, "%d%%", ecuValue.throttle);
			LCD_Send_String(ecu_buf);
			break;
    	case 5:
    		sprintf(ecu_buf, "%d%%", ecuValue.o2_voltage);
			LCD_Send_String(ecu_buf);
			break;
    }
}

