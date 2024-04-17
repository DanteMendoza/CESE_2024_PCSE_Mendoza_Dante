/**
 ******************************************************************************
 * @file    API_hcsr04.h
 * @author  Mendoza, Dante.
 * @brief   Modulo Sensor ultrasónico HC-SR04.
 ******************************************************************************
 */

#ifndef API_INC_HCSR04_H_
#define API_INC_HCSR04_H_

#include "stdint.h"

void HCSR04_Init(void);
uint16_t HCSR04_Get_Distance(void);

#endif
