/* A to D conversion of channel 0
 * This program converts the analog input from channel 0 (PTE20)
 * using software trigger continuously.
 * Bits 10-8 are used to control the tri-color LEDs. LED code * is  copied from p2_7. Connect a potentiometer between 3.3V * and  ground.
 * The wiper of the potentiometer is connected to PTE20.
 When the potentiometer is turned, the LEDs should change   color. */

#include "MKL25Z4.h"
void ADC0_init(void);

// Delays
void delayMs(int n);
void delayUs(int n);

// LCD
void LCD_command(unsigned char command);
void LCD_init(void);
void LCD_nibble_write(unsigned char data, unsigned char control);
void LCD_data(unsigned char data);
void LCD_print(char *text);

/* --- DEFINICIONES DE PINES PARA LCD --- */
#define RS 0x04 /* PTA2 */
#define EN 0x20 /* PTA5 */

// Define reference voltage constant
float Vref = 3.3;

int main(void) {

	short int result;
	short int Vin;

	short int enteros, decis, milis;

	ADC0_init(); /* Configure ADC0 */
	LCD_init(); // Configure LCD

	LCD_command(0x01); // Clear display
	delayMs(2);

	// Texto
	for(;;)
	{


		LCD_command(0x80); // Set cursor at first line

		LCD_print("Voltaje = ");

		// Complete data extraction
		ADC0->SC1[0] = 0; /* start conversion on channel 0 */

		while (!(ADC0->SC1[0] & 0x80)) {
		} /* wait COCO */
		result = ADC0->R[0];
		/* read conversion result and clear COCO flag */

		// Conversion to input voltage
		Vin = (result * Vref * 1000) / 4096;

		enteros = Vin / 1000;
		decis = (Vin / 100) % 10;
		milis = (Vin / 10) % 10;

		LCD_data('0' + enteros);
		LCD_data('.');
		LCD_data('0' + decis);
		LCD_data('0' + milis);

		delayMs(50);
	}
}

void ADC0_init(void) {

	SIM->SCGC5 |= 0x2000; /* clock to PORTE */

	PORTE->PCR[20] = 0; /* PTE20 analog input */

	SIM->SCGC6 |= 0x8000000; /* clock to ADC0 */

	ADC0->SC2 &= ~0x40; /* software trigger */

	/* clock div by 4, long sample time, single ended 12 bit, bus clock */

	ADC0->CFG1 = 0x40 | 0x10 | 0x04 | 0x00;
}

void LCD_nibble_write(unsigned char data, unsigned char control) {
    // 1. Limpiamos SOLO la basura de los pines 4 al 7
    PTD->PCOR = 0xF0;

    // 2. Colocamos el dato SOLO en los pines 4 al 7
    PTD->PSOR = (data & 0xF0);

    // 3. Control de RS (Registro: 0=Comando, 1=Dato)
    if (control & RS)
        PTA->PSOR = RS;
    else
        PTA->PCOR = RS;

    // 4. Pulso de Enable (El "clic" para que el LCD guarde el dato)
    PTA->PSOR = EN;
    delayUs(50);   // Tiempo necesario para que el pulso sea detectado
    PTA->PCOR = EN;
    delayUs(50);   // Tiempo extra de estabilización
}

void LCD_command(unsigned char command) {
    // Mandamos el nibble ALTO
    LCD_nibble_write(command & 0xF0, 0);
    // Mandamos el nibble BAJO desplazado a la posición alta
    LCD_nibble_write((command << 4) & 0xF0, 0);

    // Comandos de limpieza de pantalla tardan más
    if (command < 4)
        delayMs(5);
    else
        delayUs(100);
}

void LCD_data(unsigned char data) {
    // Misma lógica del comando, pero con la bandera RS encendida
    LCD_nibble_write(data & 0xF0, RS);
    LCD_nibble_write((data << 4) & 0xF0, RS);
    delayUs(100);
}

void LCD_print(char *text){
    // Print string character by character
    while(*text){
        LCD_data(*text++);
    }
}

void LCD_init(void) {
    SIM->SCGC5 |= 0x1000; // Reloj Port D
    SIM->SCGC5 |= 0x0200; // Reloj Port A

    // Configurar PTD4-7 como salidas
    PORTD->PCR[4] = 0x100;
    PORTD->PCR[5] = 0x100;
    PORTD->PCR[6] = 0x100;
    PORTD->PCR[7] = 0x100;
    PTD->PDDR |= 0xF0;

    // Configurar PTA2 y PTA5 como salidas
    PORTA->PCR[2] = 0x100;
    PORTA->PCR[5] = 0x100;
    PTA->PDDR |= 0x24;

    delayMs(50); // Esperar a que el voltaje del LCD se estabilice

    // --- SECUENCIA DE INICIALIZACIÓN MODO 4 BITS (ESTRICTA) ---
    LCD_command(0x30);
    delayMs(5);
    LCD_command(0x30);
    delayUs(200);
    LCD_command(0x30);
    delayUs(200);

    LCD_command(0x20); // Establecer modo 4 bits (1 solo nibble)
    delayMs(5);

    // --- CONFIGURACIÓN DE PARÁMETROS ---
    LCD_command(0x28);    // 2 líneas, fuente 5x7
    LCD_command(0x0C);    // Display ON, Cursor OFF
    LCD_command(0x01);    // Limpiar pantalla
    delayMs(5);
    LCD_command(0x06);    // Auto-incrementar cursor (escribir de izq a der)
}

void delayMs(int n){
    int i;
    // Asumiendo reloj de ~48 MHz
    SysTick->LOAD = 48000 - 1;
    SysTick->CTRL = 0x5;

    for(i = 0; i < n; i++) {
        while((SysTick->CTRL & 0x10000) == 0) { }
    }
    SysTick->CTRL = 0;
}

void delayUs(int n) {
    int i;
    // Asumiendo reloj de ~48 MHz
    SysTick->LOAD = 48 - 1;
    SysTick->CTRL = 0x5;

    for(i = 0; i < n; i++) {
        while((SysTick->CTRL & 0x10000) == 0) { }
    }
    SysTick->CTRL = 0;
}
