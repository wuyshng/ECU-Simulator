# ECU-Simulator
This project simulate Engine Control Unit (ECU) including: Engine speed, Vehicle speed, Engine coolant temp, MAF sensor, Throttle position sensor, and Oxygen sensor. It can be used for testing and development of OBD devices and software.

# Support PIDs
* 0x00: PIDs supported 
* 0x05: Coolant Temperature in on Celsius
* 0x0C: Engine speed
* 0x0D: Vehicle Speed
* 0x10: Mass air flow sensor (MAF)
* 0x11: Throttle
* 0x14: Oxygen Sensor 1

# Hardware
* STM32F103C8T6 (Blue Pill)
* TJA1050 High speed CAN transceiver
* Female OBDII socket with 12v supply to interface
* 6 potentiometers for PID adjustment 
* LCD 16x2
* 12 VDC supply
![20240602_162811](https://github.com/wuyshng/ECU-Simulator/assets/132085105/db1cf5fc-cbe8-4d2d-90ed-04772c12b48b)


# Features:
* Show current data
* Show and clear Diagnostic Trouble Codes (DTC)

# Source
[OBD-II PIDs](https://en.wikipedia.org/wiki/OBD-II_PIDs)\
[DTC (DIAGNOSTIC TROUBLE CODES) LIST](https://www.launchtech.co.uk/support/information/dtc-codes-list/)
