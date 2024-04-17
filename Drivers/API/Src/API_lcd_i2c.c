/**
 ******************************************************************************
 * @file    API_lcd_i2c.c
 * @author  Mendoza, Dante.
 * @brief   Pantalla LCD HD44780 con interfaz I2C.
 ******************************************************************************
 */

#include <API_lcd_i2c.h>

extern I2C_HandleTypeDef hi2c1;

uint8_t dpFunction;
uint8_t dpControl;
uint8_t dpMode;
uint8_t dpRows;
uint8_t dpBacklight;

static void SendCommand(uint8_t);
static void SendChar(uint8_t);
static void Send(uint8_t, uint8_t);
static void Write4Bits(uint8_t);
static void ExpanderWrite(uint8_t);
static void PulseEnable(uint8_t);
static void DelayInit(void);
static void DelayUS(uint32_t);

uint8_t special1[8] = { 0b00000, 0b11001, 0b11011, 0b00110, 0b01100, 0b11011,
		0b10011, 0b00000 };

uint8_t special2[8] = { 0b11000, 0b11000, 0b00110, 0b01001, 0b01000, 0b01001,
		0b00110, 0b00000 };

/**
 * @brief Inicializa la pantalla LCD HD44780 con interfaz I2C.
 * @param rows El número de filas de la pantalla (1 o 2).
 * Esta función inicializa la pantalla LCD HD44780 con la interfaz I2C.
 * Se deben especificar el número de filas de la pantalla (1 o 2).
 * @return None.
 */
void HD44780_Init(uint8_t rows) {
	dpRows = rows;

	dpBacklight = LCD_BACKLIGHT;

	dpFunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;

	if (dpRows > 1) {
		dpFunction |= LCD_2LINE;
	} else {
		dpFunction |= LCD_5x10DOTS;
	}

	/* Wait for initialization */
	DelayInit();
	HAL_Delay(50);

	ExpanderWrite(dpBacklight);
	HAL_Delay(1000);

	/* 4bit Mode */
	Write4Bits(0x03 << 4);
	DelayUS(4500);

	Write4Bits(0x03 << 4);
	DelayUS(4500);

	Write4Bits(0x03 << 4);
	DelayUS(4500);

	Write4Bits(0x02 << 4);
	DelayUS(100);

	/* Display Control */
	SendCommand(LCD_FUNCTIONSET | dpFunction);

	dpControl = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	HD44780_Display();
	HD44780_Clear();

	/* Display Mode */
	dpMode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	SendCommand(LCD_ENTRYMODESET | dpMode);
	DelayUS(4500);

	HD44780_CreateSpecialChar(0, special1);
	HD44780_CreateSpecialChar(1, special2);

	HD44780_Home();
}

/**
 * @brief Borra el contenido de la pantalla LCD.
 * Esta función borra todo el contenido de la pantalla LCD.
 * @return None.
 */
void HD44780_Clear() {
	SendCommand(LCD_CLEARDISPLAY);
	DelayUS(2000);
}

/**
 * @brief Mueve el cursor a la posición de inicio en la pantalla LCD.
 * Esta función mueve el cursor a la posición de inicio en la pantalla LCD, que es la posición (0, 0)
 * en la primera fila. También borra cualquier contenido mostrado en la pantalla después de realizar
 * la operación de retorno al inicio.
 * @return None.
 * Ejemplo de uso:
 * HD44780_Home();
 */
void HD44780_Home() {
	SendCommand(LCD_RETURNHOME);
	DelayUS(2000);
}

/**
 * @brief Establece el cursor en la posición especificada.
 * @param col La columna donde se posicionará el cursor (0-indexed).
 * @param row La fila donde se posicionará el cursor (0-indexed).
 * Esta función mueve el cursor a la posición especificada por la columna y la fila en la pantalla LCD.
 * @return None.
 * Ejemplo de uso:
 * HD44780_SetCursor(0, 0); // Mueve el cursor a la primera posición de la primera fila.
 */
void HD44780_SetCursor(uint8_t col, uint8_t row) {
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if (row >= dpRows) {
		row = dpRows - 1;
	}
	SendCommand(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

/**
 * @brief Apaga la visualización de la pantalla LCD.
 * @return None.
 */
void HD44780_NoDisplay() {
	dpControl &= ~LCD_DISPLAYON;
	SendCommand(LCD_DISPLAYCONTROL | dpControl);
}

/**
 * @brief Enciende la visualización de la pantalla LCD.
 * @return None.
 */
void HD44780_Display() {
	dpControl |= LCD_DISPLAYON;
	SendCommand(LCD_DISPLAYCONTROL | dpControl);
}

/**
 * @brief Oculta el cursor en la pantalla LCD.
 * @return None.
 */
void HD44780_NoCursor() {
	dpControl &= ~LCD_CURSORON;
	SendCommand(LCD_DISPLAYCONTROL | dpControl);
}

/**
 * @brief Muestra el cursor en la pantalla LCD.
 * @return None.
 */
void HD44780_Cursor() {
	dpControl |= LCD_CURSORON;
	SendCommand(LCD_DISPLAYCONTROL | dpControl);
}

/**
 * @brief Desactiva el parpadeo del cursor en la pantalla LCD.
 * @return None.
 */
void HD44780_NoBlink() {
	dpControl &= ~LCD_BLINKON;
	SendCommand(LCD_DISPLAYCONTROL | dpControl);
}

/**
 * @brief Activa el parpadeo del cursor en la pantalla LCD.
 * @return None.
 */
void HD44780_Blink() {
	dpControl |= LCD_BLINKON;
	SendCommand(LCD_DISPLAYCONTROL | dpControl);
}

/**
 * @brief Desplaza el contenido de la pantalla LCD hacia la izquierda.
 * @return None.
 */
void HD44780_ScrollDisplayLeft(void) {
	SendCommand(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

/**
 * @brief Desplaza el contenido de la pantalla LCD hacia la derecha.
 * @return None.
 */
void HD44780_ScrollDisplayRight(void) {
	SendCommand(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

/**
 * @brief Establece la dirección de escritura de la pantalla LCD de izquierda a derecha.
 * @return None.
 */
void HD44780_LeftToRight(void) {
	dpMode |= LCD_ENTRYLEFT;
	SendCommand(LCD_ENTRYMODESET | dpMode);
}

/**
 * @brief Establece la dirección de escritura de la pantalla LCD de derecha a izquierda.
 * @return None.
 */
void HD44780_RightToLeft(void) {
	dpMode &= ~LCD_ENTRYLEFT;
	SendCommand(LCD_ENTRYMODESET | dpMode);
}

/**
 * @brief Activa el desplazamiento automático del contenido de la pantalla LCD.
 * @return None.
 */
void HD44780_AutoScroll(void) {
	dpMode |= LCD_ENTRYSHIFTINCREMENT;
	SendCommand(LCD_ENTRYMODESET | dpMode);
}

/**
 * @brief Desactiva el desplazamiento automático del contenido de la pantalla LCD.
 * @return None.
 */
void HD44780_NoAutoScroll(void) {
	dpMode &= ~LCD_ENTRYSHIFTINCREMENT;
	SendCommand(LCD_ENTRYMODESET | dpMode);
}

/**
 * @brief Crea un carácter especial en la memoria CGRAM de la pantalla LCD.
 * @param location Posición de memoria donde se almacenará el carácter (0-7).
 * @param charmap  Matriz de bytes que define la forma del carácter especial.
 * @return None.
 */
void HD44780_CreateSpecialChar(uint8_t location, uint8_t charmap[]) {
	location &= 0x7;
	SendCommand(LCD_SETCGRAMADDR | (location << 3));
	for (int i = 0; i < 8; i++) {
		SendChar(charmap[i]);
	}
}

/**
 * @brief Imprime un carácter especial en la pantalla LCD.
 * @param index Índice del carácter especial creado previamente.
 * @return None.
 */
void HD44780_PrintSpecialChar(uint8_t index) {
	SendChar(index);
}

/**
 * @brief Carga un carácter personalizado en la memoria CGRAM de la pantalla LCD.
 * @param char_num Número del carácter personalizado (0-7).
 * @param rows Matriz de bytes que define la forma del carácter personalizado.
 * @return None.
 */
void HD44780_LoadCustomCharacter(uint8_t char_num, uint8_t *rows) {
	HD44780_CreateSpecialChar(char_num, rows);
}

/**
 * @brief Imprime una cadena de caracteres en la pantalla LCD.
 * @param c Cadena de caracteres a imprimir.
 * @return None.
 */
void HD44780_PrintStr(const char c[]) {
	while (*c)
		SendChar(*c++);
}

/**
 * @brief Establece el estado del retroiluminación de la pantalla LCD.
 * @param new_val Nuevo valor del estado del retroiluminación (0 o 1).
 * @return None.
 */
void HD44780_SetBacklight(uint8_t new_val) {
	if (new_val)
		HD44780_Backlight();
	else
		HD44780_NoBacklight();
}

/**
 * @brief Apaga el retroiluminación de la pantalla LCD.
 * @return None.
 */
void HD44780_NoBacklight(void) {
	dpBacklight = LCD_NOBACKLIGHT;
	ExpanderWrite(0);
}

/**
 * @brief Enciende el retroiluminación de la pantalla LCD.
 * @return None.
 */
void HD44780_Backlight(void) {
	dpBacklight = LCD_BACKLIGHT;
	ExpanderWrite(0);
}

/**
 * @brief Envía un comando a la pantalla LCD.
 * @param cmd Comando a enviar.
 * @return None.
 */
static void SendCommand(uint8_t cmd) {
	Send(cmd, 0);
}

/**
 * @brief Envía un carácter a la pantalla LCD.
 * @param ch Carácter a enviar.
 * @return None.
 */
static void SendChar(uint8_t ch) {
	Send(ch, RS);
}

/**
 * @brief Envía datos a la pantalla LCD.
 * @param value Valor a enviar.
 * @param mode Modo de envío (RS: 1 para datos, 0 para comandos).
 * @return None.
 */
static void Send(uint8_t value, uint8_t mode) {
	uint8_t highnib = value & 0xF0;
	uint8_t lownib = (value << 4) & 0xF0;
	Write4Bits((highnib) | mode);
	Write4Bits((lownib) | mode);
}

/**
 * @brief Envía los 4 bits superiores a la pantalla LCD.
 * @param value Valor de los 4 bits superiores a enviar.
 * @return None.
 */
static void Write4Bits(uint8_t value) {
	ExpanderWrite(value);
	PulseEnable(value);
}

/**
 * @brief Escribe datos en el expansor de E/S de la pantalla LCD.
 * @param _data Datos a escribir.
 * @return None.
 */
static void ExpanderWrite(uint8_t _data) {
	uint8_t data = _data | dpBacklight;
	HAL_I2C_Master_Transmit(&hi2c1, DEVICE_ADDR, (uint8_t*) &data, 1, 10);
}

/**
 * @brief Genera un pulso de habilitación para la pantalla LCD.
 * @param _data Datos del pulso de habilitación.
 * @return None.
 */
static void PulseEnable(uint8_t _data) {
	ExpanderWrite(_data | ENABLE);
	DelayUS(20);

	ExpanderWrite(_data & ~ENABLE);
	DelayUS(20);
}

/**
 * @brief Inicializa el sistema de retardo utilizando el contador de ciclos de CPU.
 *        Esta función debe ser llamada antes de utilizar la función de retardo.
 * @return None.
 */
static void DelayInit(void) {
	CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk;
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

	DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk; //~0x00000001;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk; //0x00000001;

	DWT->CYCCNT = 0;

	/* 3 NO OPERATION instructions */
	__ASM volatile ("NOP");
	__ASM volatile ("NOP");
	__ASM volatile ("NOP");
}

/**
 * @brief Genera un retardo en microsegundos utilizando el contador de ciclos de CPU.
 * @param us: Número de microsegundos de retardo deseado.
 * @return None.
 */
static void DelayUS(uint32_t us) {
	uint32_t cycles = (SystemCoreClock / 1000000L) * us;
	uint32_t start = DWT->CYCCNT;
	volatile uint32_t cnt;

	do {
		cnt = DWT->CYCCNT - start;
	} while (cnt < cycles);
}
