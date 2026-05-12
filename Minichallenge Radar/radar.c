#include "MKL25Z4.h"
#include <stdio.h>

// --- Configuración de Pines ---
// Motor: PTB0, PTB1, PTB2, PTB3
// Sensor: PTC1 (Trig), PTC2 (Echo)
// LED: PTD1 (Azul)

uint8_t secuencia[] = {0x09, 0x0C, 0x06, 0x03};

// --- Prototipos ---
void UART0_init(void);
void UART0_send_string(char* str);
void init_all(void);
void delay_ms(uint32_t ms);
void delay_us(uint32_t us);
float medirDistancia(void);
void UART0_send_float(float f);

// --- Función para enviar texto manualmente por el puerto COM ---
void UART0_send_string(char* str) {
    while(*str) {
        while(!(UART0->S1 & UART0_S1_TDRE_MASK)); // Esperar a que esté libre
        UART0->D = *str++; // Enviar caracter
    }
}

void UART0_init(void) {
    SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
    SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1); // Reloj 21MHz
    PORTA->PCR[1] = PORT_PCR_MUX(2); // RX
    PORTA->PCR[2] = PORT_PCR_MUX(2); // TX
    UART0->BDH = 0x00;
    UART0->BDL = 136; // 9600 baudios
    UART0->C4 = 15;
    UART0->C2 |= (UART0_C2_TE_MASK | UART0_C2_RE_MASK);
}

void init_all(void) {
    UART0_init();
    SIM->SCGC5 |= (SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK);

    // Configurar Motor (PTB0-PTB3)
    PTB->PDDR |= 0x0F;
    for(int i=0; i<4; i++) PORTB->PCR[i] = PORT_PCR_MUX(1);

    // Configurar Sensor (PTC1 Trig, PTC2 Echo)
    PORTC->PCR[1] = PORT_PCR_MUX(1);
    PORTC->PCR[2] = PORT_PCR_MUX(1);
    PTC->PDDR |= (1 << 1);  // Trig Salida
    PTC->PDDR &= ~(1 << 2); // Echo Entrada

    // Configurar LED Azul (PTD1)
    PORTD->PCR[1] = PORT_PCR_MUX(1);
    PTD->PDDR |= (1 << 1);
    PTD->PSOR = (1 << 1); // Apagado inicialmente

    // Configurar SysTick
    SysTick->LOAD = 0xFFFFFF;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
}

float medirDistancia(void) {
    PTC->PCOR |= (1 << 1); // Trigger LOW
    delay_us(2);
    PTC->PSOR |= (1 << 1); // Trigger HIGH
    delay_us(10);
    PTC->PCOR |= (1 << 1); // Trigger LOW

    uint32_t timeout = 1000000;
    while(!(PTC->PDIR & (1 << 2)) && timeout--); // Esperar subida
    uint32_t start = SysTick->VAL;

    timeout = 1000000;
    while((PTC->PDIR & (1 << 2)) && timeout--); // Esperar bajada
    uint32_t stop = SysTick->VAL;

    uint32_t delta = (start - stop) & 0xFFFFFF;
    float d = (float)delta / 1218.0;
    return (d > 50.0 || d < 0) ? 50.0 : d;
}

void delay_us(uint32_t us) {
    volatile uint32_t i;
    while(us--) for(i = 0; i < 20; i++);
}

void delay_ms(uint32_t ms) {
    while(ms--) delay_us(1000);
}

// Agrega estas funciones arriba del main para enviar números sin sprintf
void UART0_send_float(float f) {
    char buf[10];
    int entero = (int)f;
    int decimal = (int)((f - entero) * 10); // 1 decimal de precisión
    sprintf(buf, "%d.%d", entero, decimal);
    UART0_send_string(buf);
}

int main(void) {
    init_all();
    int paso = 0;
    int pasos_180 = 1024;

    while(1) {
        // --- IDA ---
        for (int r = 0; r < pasos_180; r++) {
            paso = (paso + 1) % 4;
            PTB->PDOR = (PTB->PDOR & ~0x0F) | secuencia[paso];
            delay_ms(7);

            if(r % 25 == 0) {
                float angulo = (float)r * (180.0 / 1024.0);
                float dist = medirDistancia();

                // Enviamos los datos uno por uno
                UART0_send_float(angulo);
                UART0_send_string(",");
                UART0_send_float(dist);
                UART0_send_string("\r\n");

                PTD->PTOR = (1 << 1);
            }
        }

        // --- REGRESO ---
        for (int r = pasos_180; r > 0; r--) {
            paso = (paso == 0) ? 3 : paso - 1;
            PTB->PDOR = (PTB->PDOR & ~0x0F) | secuencia[paso];
            delay_ms(7);

            if(r % 25 == 0) {
                float angulo = (float)r * (180.0 / 1024.0);
                float dist = medirDistancia();

                UART0_send_float(angulo);
                UART0_send_string(",");
                UART0_send_float(dist);
                UART0_send_string("\r\n");

                PTD->PTOR = (1 << 1);
            }
        }
    }
}
