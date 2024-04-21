/**
 ******************************************************************************
 * @file    API_hcsr04.c
 * @author  Mendoza, Dante.
 * @brief   Modulo Sensor ultrasónico HC-SR04.
 ******************************************************************************
 */

#include <API_hcsr04_port.h>
#include "tim.h"

uint32_t t_ini = 0;
uint32_t t_end = 0;
uint32_t t_time = 0;
uint16_t dist = 0;
uint8_t flag_captured = 0;

/**
 * @brief Callback de captura de señal para el temporizador de hardware (TIM).
 * Esta función se llama automáticamente cuando se produce una captura de señal en el temporizador de hardware.
 * Calcula la distancia utilizando la señal capturada y actualiza la variable global 'dist'.
 * No se llama directamente desde el código; se llama automáticamente por el hardware del temporizador.
 * @param htim: Puntero al controlador de temporizador de hardware que generó la interrupción.
 * @return None.
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
		if (flag_captured == 0) {
			t_ini = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			flag_captured = 1;
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1,
					TIM_INPUTCHANNELPOLARITY_FALLING);
		}

		else if (flag_captured == 1) {
			t_end = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			__HAL_TIM_SetCounter(htim, 0);

			if (t_end > t_ini) {
				t_time = t_end - t_ini;
			} else if (t_ini > t_end) {
				t_time = (0xFFFF - t_ini) + t_end;
			}

			dist = (uint16_t) ((t_time * 0.034) / 2);
			flag_captured = 0;

			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1,
					TIM_INPUTCHANNELPOLARITY_RISING);
			__HAL_TIM_DISABLE_IT(&htim1, TIM_IT_CC1);
		}
	}
}
