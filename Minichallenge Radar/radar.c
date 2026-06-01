#include <MKL25Z4.h>
#include <stdio.h>

// Variables globales
volatile int msTicks = 0;
const uint8_t step_sequence[4] = {0x02, 0x04, 0x08, 0x10}; // PTB1 a PTB4

// --- PROTOTIPOS ---
void delay_ms(int ms);
void delay_us(int us);
void UART0_Init(void);
void UART0_SendString(char *s);
void ADC0_Init(void);
uint16_t ADC0_Read(void);
int Medir_Distancia(void);

// --- INTERRUPCIÓN SYSTICK ---
void SysTick_Handler(void) { msTicks++; }

// --- CONFIGURACIÓN PRINCIPAL ---
int main(void) {
    // Configurar SysTick a 1ms
    SysTick->LOAD = 20999;
    SysTick->VAL = 0;
    SysTick->CTRL = 0x07;

    UART0_Init();
    ADC0_Init();

    // Configurar Reloj para Puerto B
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;

    // Configurar MUX Pines
    PORTB->PCR[0] = PORT_PCR_MUX(0); // PTB0: ADC (Red Pitaya)
    PORTB->PCR[1] = PORT_PCR_MUX(1); // PTB1: Motor
    PORTB->PCR[2] = PORT_PCR_MUX(1); // PTB2: Motor
    PORTB->PCR[3] = PORT_PCR_MUX(1); // PTB3: Motor
    PORTB->PCR[4] = PORT_PCR_MUX(1); // PTB4: Motor
    PORTB->PCR[8] = PORT_PCR_MUX(1); // PTB8: Trig (Salida)
    PORTB->PCR[9] = PORT_PCR_MUX(1); // PTB9: Echo (Entrada)

    // Direcciones (1=Salida, 0=Entrada) -> Pines 1,2,3,4 y 8 como salidas
    PTB->PDDR |= (1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<8);
    PTB->PDDR &= ~(1<<9); // Echo entrada

    char buffer[40];
    int angulo = 0;
    int direccion = 1; // 1 = derecha, -1 = izquierda
    int step = 0;

    while (1) {
        // 1. Leer Red Pitaya (Modifica la velocidad del radar)
        uint16_t velocidad_rp = ADC0_Read();
        int delay_motor = 5 + (velocidad_rp / 1500); // Ajusta el retardo (más bajo = más rápido)

        // 2. Medir Distancia (Ultrasonido)
        int distancia = Medir_Distancia();

        // 3. Enviar Datos al PC
        sprintf(buffer, "%d,%d,%d\n", angulo, distancia, delay_motor);
        UART0_SendString(buffer);

        // 4. Mover Motor un paso
        PTB->PCOR = 0x1E; // Limpiar PTB1-4
        PTB->PSOR = step_sequence[step];
        delay_ms(delay_motor); // La Red Pitaya dicta qué tan rápido gira

        step++;
        if (step >= 4) step = 0;

        // 5. Actualizar Ángulo (0 a 180 grados y viceversa)
        angulo += direccion;
        if (angulo >= 180 || angulo <= 0) {
            direccion *= -1; // Invertir giro para el barrido
        }
    }
}

// --- FUNCIONES AUXILIARES ---
int Medir_Distancia(void) {
    // Pulso Trig de 10us
    PTB->PSOR = (1<<8);
    delay_us(10);
    PTB->PCOR = (1<<8);

    // Esperar a que Echo suba
    int timeout = 0;
    while (!(PTB->PDIR & (1<<9))) {
        timeout++;
        if(timeout > 100000) return 0;
    }

    // Contar duración del pulso Echo
    int tiempo = 0;
    while (PTB->PDIR & (1<<9)) {
        tiempo++;
        delay_us(1);
        if(tiempo > 30000) break; // Limitar rango máximo
    }

    // Calcular en CM (División aproximada basada en la velocidad del sonido)
    return tiempo / 58;
}

void ADC0_Init(void) {
    SIM->SCGC6 |= (1UL << 27);
    ADC0->CFG1 = 0x0C;
    ADC0->SC3 = 0;
}

uint16_t ADC0_Read(void) {
    ADC0->SC1[0] = 8; // Canal 8 (PTB0)
    while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));
    return ADC0->R[0];
}

void UART0_Init(void) {
    SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
    SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1);
    PORTA->PCR[1] = PORT_PCR_MUX(2);
    PORTA->PCR[2] = PORT_PCR_MUX(2);
    UART0->C2 = 0;
    UART0->BDH = 0x00;
    UART0->BDL = 0x1A; // ~115200 baudios
    UART0->C2 = UART_C2_TE_MASK | UART_C2_RE_MASK;
}

void UART0_SendString(char *s) {
    while (*s) {
        while (!(UART0->S1 & UART_S1_TDRE_MASK));
        UART0->D = *s++;
    }
}

void delay_ms(int ms) {
    int start = msTicks;
    while ((msTicks - start) < ms);
}

void delay_us(int us) {
    // Retardo simple (apróximado para 48MHz)
    for (volatile int i = 0; i < (us * 12); i++);
}
