/**
 ******************************************************************************
 * @file    API_hcsr04FSM.h
 * @author  Mendoza, Dante.
 * @brief   Maquina de estados finitos Sensor ultrasónico HC-SR04.
 ******************************************************************************
 */

#ifndef API_HCSR04FSM_H
#define API_HCSR04FSM_H

#include <stdint.h>

// Definición del tipo de estado
typedef enum {
    NEAR_STATE,
    NORMAL_STATE,
    FAR_STATE
} hcsr04State_t;

// Prototipos de funciones
void hcsr04FSM_init(void);
void hcsr04FSM_update(uint16_t distance);

#endif /* API_HCSR04FSM_H */
