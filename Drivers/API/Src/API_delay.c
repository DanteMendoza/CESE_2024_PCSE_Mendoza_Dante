/**
 ******************************************************************************
 * @file    API_delay.c
 * @author  Mendoza, Dante.
 * @brief   delay no bloqueante.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "API_delay.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MINIMUM_DURATION 1
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* UART handler declaration */
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief inicializa la estructura que maneja el delay no bloqueante.
 * @param *delay: puntero a la estructura del delay.
 * @param duration: duracion en milisegundos.
 * @return None.
 */
void delayInit(delay_t *delay, tick_t duration) {
	if (delay != NULL && duration >= MINIMUM_DURATION) {
		delay->duration = duration;
		delay->running = false;
	}
}

/*
 * @brief Esta función se utiliza para consultar si el delay ha completado la duración requerida.
 * En caso afirmativo, devuelve Verdadero, caso contrario devuelve Falso.
 * Si es la primera vez que se llama a la función, se pone delay->running = Verdadero.
 * @param *delay: puntero a la estructura del delay.
 * @return bool.
 */
bool_t delayRead(delay_t *delay) {
	if (delay != NULL) {
		if (!delay->running) {
			// Tomar marca de tiempo
			delay->startTime = HAL_GetTick();
			// Cambiar el flag a 'true'
			delay->running = true;
			return false;
		}

		if (delay->duration <= (HAL_GetTick() - delay->startTime)) {
			delay->running = false;
			return true;  // El tiempo se cumplió.
		}

	}

	return false;  // El tiempo no se ha cumplido.
}

/**
 * @brief Esta función se utiliza para cambiar la duración de un delay ya inicializado.
 * @param *delay: puntero a la estructura del delay.
 * @param duration: duracion en milisegundos.
 * @return None.
 */
void delayWrite(delay_t *delay, tick_t duration) {
	if (delay != NULL && duration >= MINIMUM_DURATION) {
		delay->duration = duration;
	}
}

