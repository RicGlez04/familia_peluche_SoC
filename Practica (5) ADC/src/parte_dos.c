/* A to D conversion of channel 0
 * This program converts the analog input from channel 0 (PTE20)
 * using software trigger continuously.
 * Bits 10-8 are used to control the tri-color LEDs. LED code * is  copied from p2_7. Connect a potentiometer between 3.3V * and  ground.
 * The wiper of the potentiometer is connected to PTE20.
 When the potentiometer is turned, the LEDs should change   color. */

#include "MKL25Z4.h"
void ADC0_init(void);
void LED_set(int s);
void LED_init(void);

//LCD
void LCD_command(unsigned char command);
void LCD_init(void);
void LCD_nibble_write(unsigned char data, unsigned char control);
void LCD_data(unsigned char data);
void LCD_print(char *text);


/* --- PROTOTIPOS DE FUNCIONES --- */
void delayMs(int n);
void delayUs(int n);

/* --- DEFINICIONES DE PINES PARA LCD --- */
#define RS 0x04 /* PTA2 */
#define EN 0x20 /* PTA5 */

int main(void) {
	short int result;
	LED_init(); /* Configure LEDs */
	ADC0_init(); /* Configure ADC0 */
	LCD_init();

	    // Imprimir un mensaje de prueba inicial
	    LCD_command(0x80); // Ir a la primera línea, primera posición


	while (1) {
		ADC0->SC1[0] = 0; /* Inicia conversión */
		    while (!(ADC0->SC1[0] & 0x80)) { }
		    result = ADC0->R[0];

		    // 1. Convertir a voltaje (0 a 330 para representar 3.30V)
		    // Usamos 330 para tener dos decimales de precisión
		    int voltage = (result * 330) / 4095;
		    int luz = (voltage*100)/330;

		    int U = luz / 100;          // Unidades (ej: 3)
		    int D = (luz / 10) % 10;    // Primer decimal (ej: 2)
		    int C = luz % 10;           // Segundo decimal (ej: 5)

		    // 2. Imprimir en LCD
		    LCD_command(0x80);              // Primera línea
		    LCD_print("porcentaje: ");

		    LCD_command(0xC0);              // Segunda línea
		    LCD_data(U + '0');              // Convertir int a ASCII ('0' = 48)
	               // Punto decimal
		    LCD_data(D + '0');
		    LCD_data(C + '0');
		    LCD_print(" Luz");             // Espacios extra para limpiar basura previa

		    if (luz < 20){ /* use bit 0 of s to control red LED */
		    		PTB->PCOR = 0x40000; /* turn on red LED */
		    		delayMs(2300);
		    	}
		    	else
		    		PTB->PSOR = 0x40000; /* turn off red LED */

		    delayMs(100);
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
void LED_init(void) {

	SIM->SCGC5 |= 0x400; /* enable clock to Port B */

	SIM->SCGC5 |= 0x1000; /* enable clock to Port D */

	PORTB->PCR[18] = 0x100; /* make PTB18 pin as GPIO */

	PTB->PDDR |= 0x40000; /* make PTB18 as output pin */

	PORTB->PCR[19] = 0x100; /* make PTB19 pin as GPIO */

	PTB->PDDR |= 0x80000; /* make PTB19 as output pin */

	PORTD->PCR[1] = 0x100; /* make PTD1 pin as GPIO */

	PTD->PDDR |= 0x02;
	// APAGAR TODO POR DEFECTO
	    PTB->PSOR = 0x40000 | 0x80000;
	    PTD->PSOR = 0x02;
} /* make PTD1 as output pin */
void LED_set(int s) {

	if (s == 100){ /* use bit 0 of s to control red LED */
		PTB->PCOR = 0x40000; /* turn on red LED */
		delayMs(2000);
	}
	else
		PTB->PSOR = 0x40000; /* turn off red LED */
}


/* =========================================
 * FUNCIONES DEL LCD (MODO 4 BITS CORREGIDO)
 * ========================================= */

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
//////////////////////////////////

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
