/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    can.h
 * @brief   This file contains all the function prototypes for
 *          the can.c file
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CAN_H__
#define __CAN_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

    /* USER CODE BEGIN Includes */

    /* USER CODE END Includes */

    extern CAN_HandleTypeDef hcan;

    /* USER CODE BEGIN Private defines */
    extern CAN_TxHeaderTypeDef TxHeader;
    extern CAN_RxHeaderTypeDef RxHeader;

    extern int DataCheck;
    extern int obd2_check;

    typedef struct CAN_message_t
    {
        uint32_t id;        // can identifier
        uint16_t timestamp; // FlexCAN time when message arrived
        struct
        {
            uint8_t extended : 1; // identifier is extended (29-bit)
            uint8_t remote : 1;   // remote transmission request packet type
            uint8_t overrun : 1;  // message overrun
            uint8_t reserved : 5;
        } flags;
        uint8_t len; // length of data
        uint8_t buf[8];
    } CAN_message_t;

    enum CANbaudRatePrescaler
    {
        CAN_1000kbit = 2,
        CAN_500kbit = 4,
        CAN_250kbit = 8,
        CAN_125kbit = 16,
        CAN_100kbit = 20
    };
    /* USER CODE END Private defines */

    void MX_CAN_Init(void);

    /* USER CODE BEGIN Prototypes */
    void CAN_SetBaudRate(uint32_t baudrate);
    void CAN_FilterConfig(void);
    void CAN_Tx(void);
    void CAN_Tx2(void);
    void Send_Response(void);
    /* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __CAN_H__ */
