#include "MKL25Z4.h"
#include <stdio.h>

// --- Configuración de Pines ---
#define PIN_A (0) // PTB0
#define PIN_B (1) // PTB1
#define PIN_C (2) // PTB2
#define PIN_D (3) // PTB3

#define TRIG_PIN (8) // PTB8 (Físico: Conector J2 - Pin 2)
#define ECHO_PIN (9) // PTB9 (Físico: Conector J2 - Pin 4)

// Secuencia de paso completo (Full Step)
uint8_t secuencia[] = {0x09, 0x0C, 0x06, 0x03};

// --- Prototipos de Funciones ---
void UART0_init(void);
void init_all(void);
void delay_us(uint32_t us);
float leer_distancia(void);

// --- Configuración UART0 para imprimir por USB ---
void UART0_init(void) {
    SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
    SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1);

    PORTA->PCR[1] = PORT_PCR_MUX(2); // RX
    PORTA->PCR[2] = PORT_PCR_MUX(2); // TX

    UART0->BDH = 0x00;
    UART0->BDL = 11; // 115200 baudios
    UART0->C4 = 15;
    UART0->C2 |= (UART0_C2_TE_MASK | UART0_C2_RE_MASK);
}

// Redirección de printf a UART0
int fputc(int ch, FILE *f) {
    while(!(UART0->S1 & UART0_S1_TDRE_MASK));
    UART0->D = ch;
    return ch;
}

void init_all(void) {
    UART0_init();
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;

    // Configuración Motor
    PTB->PDDR |= 0x0F;
    for(int i=0; i<4; i++) PORTB->PCR[i] = PORT_PCR_MUX(1);

    // Configuración Ultrasonido
    PORTB->PCR[TRIG_PIN] = PORT_PCR_MUX(1);
    PORTB->PCR[ECHO_PIN] = PORT_PCR_MUX(1);
    PTB->PDDR |= (1 << TRIG_PIN);
    PTB->PDDR &= ~(1 << ECHO_PIN);
}

void delay_us(uint32_t us) {
    for(uint32_t i = 0; i < (us * 3); i++) __asm("nop");
}

float leer_distancia(void) {
    uint32_t tiempo = 0;
    PTB->PSOR = (1 << TRIG_PIN);
    delay_us(10);
    PTB->PCOR = (1 << TRIG_PIN);

    uint32_t timeout = 0;
    while(!(PTB->PDIR & (1 << ECHO_PIN)) && timeout < 10000) timeout++;

    while(PTB->PDIR & (1 << ECHO_PIN)) {
        tiempo++;
        delay_us(1);
    }
    return (float)tiempo * 0.017;
}

int main(void) {

	// Inicializar todos los modulos que se van a utilizar
	init_all();

	// Definir constantes
    int paso_actual = 0;
    int ciclos_vuelta = 512; // Ajuste para motor 28BYJ-48

    // Definir angulo
    float angulo = 0;

    // printf("\r\n--- TARS: Sistema de Control de Giro ---\r\n");

    while(1) {
        // --- GIRO HACIA ADELANTE ---
        // printf("> Girando adelante...\r\n");
        for (int r = 0; r < ciclos_vuelta; r++) {
            for (int i = 0; i < 4; i++) {
                paso_actual = i; // Actualización del paso actual solicitado
                PTB->PDOR = (PTB->PDOR & ~0x0F) | secuencia[paso_actual];
                delay_us(1500);
            }

            // Muestra distancia cada 50 ciclos para no frenar el motor
            if(r % 50 == 0) {
            	angulo = (360.0 / ciclos_vuelta) * r;
            	printf("%.1f,%.1f\r\n", angulo, leer_distancia());
            }
        }

        // printf("> Pausa...\r\n");
        PTB->PCOR = 0x0F; // Apagar bobinas
        delay_us(1000000); // 1 segundo

        // --- GIRO HACIA ATRÁS (Secuencia invertida) ---
        // printf("> Girando atras...\r\n");
        for (int r = 0; r < ciclos_vuelta; r++) {
            for (int i = 3; i >= 0; i--) { // Índice invertido
                paso_actual = i; // Actualización del paso actual solicitado
                PTB->PDOR = (PTB->PDOR & ~0x0F) | secuencia[paso_actual];
                delay_us(1500);
            }
            if(r % 50 == 0) {
            	angulo = (360.0 / ciclos_vuelta) * r;
            	printf("%.1f,%.1f\r\n", angulo, leer_distancia());
            }
        }
        // printf("> Ciclo terminado.\r\n");
        delay_us(1000000);
    }
}
