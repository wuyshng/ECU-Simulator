/*
 * obd.h
 *
 *  Created on: Mar 4, 2024
 *      Author: quyma
 */

#ifndef INC_OBD_H_
#define INC_OBD_H_

#include "main.h"

/* Details from http://en.wikipedia.org/wiki/OBD-II_PIDs */
#define MODE1               0x01        //Show current data
#define MODE2               0x02        //Show freeze frame data
#define MODE3               0x03        //Show stored Diagnostic Trouble Codes
#define MODE4               0x04        //Clear Diagnostic Trouble Codes and stored values

#define PID_SUPPORTED       0x00
#define MONITOR_STATUS      0x01
#define ENGINE_COOLANT_TEMP 0x05
#define ENGINE_SPEED        0x0C
#define VEHICLE_SPEED       0x0D
#define MAF_SENSOR          0x10
#define THROTTLE            0x11
#define O2_VOLTAGE          0x14

#define MODE1_RESPONSE      0x41
#define MODE2_RESPONSE      0x42
#define MODE3_RESPONSE      0x43
#define MODE4_RESPONSE      0x44
#define PID_REQUEST         0x7DF
#define PID_REPLY           0x7E8

typedef struct{
	int16_t engine_coolant_temp;
	uint16_t engine_speed;
	uint16_t vehicle_speed;
	uint16_t maf_sensor;
	uint16_t throttle;
	int16_t o2_voltage;
} OBD2Data;

typedef struct{
	int16_t engine_coolant_temp;
	uint16_t engine_speed;
	uint16_t vehicle_speed;
	uint16_t maf_sensor;
	uint16_t throttle;
	float o2_voltage;
} ECUValue;

extern OBD2Data obd2_data;
extern ECUValue ecuValue;
extern uint16_t adcScanValue[6];
extern uint8_t numberPID;
extern uint8_t PID_data[2];
extern float curPIDValue;
extern float oldPIDValue;
extern uint8_t ecuDTC;

int gen_random_number(void);
void ecu_scale_analog_value(void);
void ecu_display_LCD(uint8_t numberPID);
void ecu_send_respond(void);
long map(long inVal, long inMin, long inMax, long outMin, long outMax);
void display_ecu_value(int32_t PIDValue);

#endif /* INC_OBD_H_ */
