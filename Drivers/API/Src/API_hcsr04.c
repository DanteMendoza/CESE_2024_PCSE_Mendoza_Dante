/**
 ******************************************************************************
 * @file    API_hcsr04.c
 * @author  Mendoza, Dante.
 * @brief   Modulo Sensor ultrasónico HC-SR04.
 ******************************************************************************
 */

#include <API_hcsr04.h>
#include <API_hcsr04_port.h>
#include "tim.h"

uint16_t dists = 0;

/**
 * @brief Inicializa el sensor de distancia ultrasónico HC-SR04.
 * Esta función configura los pines GPIO y el temporizador de hardware (TIM) para el funcionamiento del sensor HC-SR04.
 * @return None.
 */
void HCSR04_Init(void) {
	HAL_GPIO_WritePin(Trigger_GPIO_Port, Trigger_Pin, GPIO_PIN_RESET);
	HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_1);
}

/**
 * @brief Obtiene la distancia medida por el sensor HC-SR04.
 * Esta función activa el disparador del sensor, mide el tiempo de vuelo de la señal ultrasónica y
 * calcula la distancia correspondiente.
 * @return La distancia medida en centímetros.
 */
uint16_t HCSR04_Get_Distance(void) {
	HAL_GPIO_WritePin(Trigger_GPIO_Port, Trigger_Pin, GPIO_PIN_SET);
	__HAL_TIM_SetCounter(&htim1, 0);
	while (__HAL_TIM_GetCounter(&htim1) < 10)
		;
	HAL_GPIO_WritePin(Trigger_GPIO_Port, Trigger_Pin, GPIO_PIN_RESET);
	/** Se habilita la interrupción del canal 1 para permitir que se capture la señal ultrasónica
	 *  y se calcule la distancia correspondiente. Esto asegura que el sistema esté preparado para
	 *  procesar la señal capturada y actualizar la distancia medida.
	 */
	__HAL_TIM_ENABLE_IT(&htim1, TIM_IT_CC1);
	return dists;
}
