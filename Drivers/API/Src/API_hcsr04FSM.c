/**
 ******************************************************************************
 * @file    API_hcsr04FSM.c
 * @author  Mendoza, Dante.
 * @brief   Maquina de estados finitos Sensor ultrasónico HC-SR04.
 ******************************************************************************
 */

#include "API_hcsr04FSM.h"

#define NEAR_VALUE 30
#define NORMAL_VALUE_LOW 31
#define NORMAL_VALUE_HIGH 69
#define FAR_VALUE 70

// Variable de estado (global)
static hcsr04State_t hcsr04State = NEAR_STATE;

/**
 * @brief Inicializa la máquina de estados finitos (FSM) del sensor HC-SR04.
 * Esta función establece el estado inicial de la FSM como NEAR_STATE.
 * @return None.
 */
void hcsr04FSM_init(void) {
	hcsr04State = NEAR_STATE;
}

/**
 * @brief Actualiza la máquina de estados finitos (FSM) del sensor HC-SR04.
 * Esta función actualiza el estado de la FSM en función de la distancia medida por el sensor.
 * @param distance: La distancia medida por el sensor HC-SR04.
 * @return None.
 * @note Esta función asume que se han definido los siguientes valores:
 * - NEAR_VALUE: Valor límite para el estado NEAR_STATE.
 * - NORMAL_VALUE_LOW: Valor inferior del rango normal de distancia.
 * - NORMAL_VALUE_HIGH: Valor superior del rango normal de distancia.
 * - FAR_VALUE: Valor límite para el estado FAR_STATE.
 */
void hcsr04FSM_update(uint16_t distance) {
    // Lógica para la transición de estados
    switch (hcsr04State) {
        case NEAR_STATE:
            if (distance <= NEAR_VALUE) {
                hcsr04State = NEAR_STATE;
            } else if (distance >= NORMAL_VALUE_LOW && distance <= NORMAL_VALUE_HIGH) {
                hcsr04State = NORMAL_STATE;
            } else {
                hcsr04State = FAR_STATE;
            }
            break;

        case NORMAL_STATE:
            if (distance <= NEAR_VALUE) {
                hcsr04State = NEAR_STATE;
            } else if (distance >= NORMAL_VALUE_LOW && distance <= NORMAL_VALUE_HIGH) {
                hcsr04State = NORMAL_STATE;
            } else {
                hcsr04State = FAR_STATE;
            }
            break;

        case FAR_STATE:
            if (distance >= FAR_VALUE) {
                hcsr04State = FAR_STATE;
            } else if (distance >= NORMAL_VALUE_LOW && distance <= NORMAL_VALUE_HIGH) {
                hcsr04State = NORMAL_STATE;
            } else {
                hcsr04State = NEAR_STATE;
            }
            break;

        default:
        	hcsr04FSM_init();
            break;
    }
}
