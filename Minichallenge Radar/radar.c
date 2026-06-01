#include <MKL25Z4.h>
#include <stdio.h>

#define IN1 (1 << 0)
#define IN2 (1 << 1)
#define IN3 (1 << 2)
#define IN4 (1 << 3)

#define TRIG (1 << 1) // PTC1
#define ECHO (1 << 2) // PTC2

// --- Funciones de Retardo ---
void delay(volatile uint32_t t) {
    while(t--) {
        for(volatile uint32_t i=0; i<1000; i++);
    }
}

void delay_us(volatile uint32_t t) {
    while(t--) {
        for(volatile uint32_t i=0; i<4; i++); // Aprox 1 microsegundo a 20.97MHz
    }
}

// --- Configuración UART0 (OpenSDA 9600 baud) ---
void UART0_Init(void) {
    SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
    PORTA->PCR[1] = PORT_PCR_MUX(2); // Rx
    PORTA->PCR[2] = PORT_PCR_MUX(2); // Tx
    SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1);
    UART0->C2 = 0;
    UART0->BDH = 0;
    UART0->BDL = 137; // 9600 baudios (asumiendo reloj FLL de 20.97 MHz)
    UART0->C4 = 0x0F;
    UART0->C2 = UART_C2_TE_MASK | UART_C2_RE_MASK;
}

void UART0_Transmit(char c) {
    while(!(UART0->S1 & UART_S1_TDRE_MASK));
    UART0->D = c;
}

void UART0_Print(char *str) {
    while(*str) {
        UART0_Transmit(*str++);
    }
}

// --- Configuración del Motor ---
void step(uint8_t s) {
    GPIOB->PCOR = IN1 | IN2 | IN3 | IN4;
    switch(s) {
        case 0: GPIOB->PSOR = IN1; break;
        case 1: GPIOB->PSOR = IN1 | IN2; break;
        case 2: GPIOB->PSOR = IN2; break;
        case 3: GPIOB->PSOR = IN2 | IN3; break;
        case 4: GPIOB->PSOR = IN3; break;
        case 5: GPIOB->PSOR = IN3 | IN4; break;
        case 6: GPIOB->PSOR = IN4; break;
        case 7: GPIOB->PSOR = IN4 | IN1; break;
    }
}

// Mueve el motor una cantidad de pasos en la dirección dada (1 = adelante, -1 = atrás)
uint8_t current_step = 0;
void move_motor(int steps, int dir) {
    for(int i = 0; i < steps; i++) {
        if(dir == 1) current_step = (current_step + 1) % 8;
        else         current_step = (current_step + 7) % 8; // Equivalente a restar 1 en módulo 8

        step(current_step);
        delay(2); // Velocidad del motor
    }
}

// --- Configuración del Ultrasónico ---
uint32_t measure_distance(void) {
    uint32_t count = 0;

    // Pulso Trigger de 10us
    GPIOC->PSOR = TRIG;
    delay_us(10);
    GPIOC->PCOR = TRIG;

    // Esperar a que Echo suba
    while(!(GPIOC->PDIR & ECHO));

    // Contar el tiempo mientras Echo esté en alto
    while((GPIOC->PDIR & ECHO)) {
        count++;
        delay_us(1);
        if(count > 30000) break; // Timeout para evitar bucles infinitos
    }

    // Convertir el conteo a distancia aprox en cm
    // (Esta constante "58" de calibración asume que 1 delay_us es casi real)
    // Puede requerir ajuste dependiendo de las optimizaciones de tu compilador.
    return count / 58;
}

int main(void) {
    // Configuración de relojes para Puertos B y C
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK;

    // Configuración pines Motor
    PORTB->PCR[0] = PORT_PCR_MUX(1);
    PORTB->PCR[1] = PORT_PCR_MUX(1);
    PORTB->PCR[2] = PORT_PCR_MUX(1);
    PORTB->PCR[3] = PORT_PCR_MUX(1);
    GPIOB->PDDR |= IN1 | IN2 | IN3 | IN4;

    // Configuración pines Ultrasónico
    PORTC->PCR[1] = PORT_PCR_MUX(1); // Trigger
    PORTC->PCR[2] = PORT_PCR_MUX(1); // Echo
    GPIOC->PDDR |= TRIG;             // Trigger salida
    GPIOC->PDDR &= ~ECHO;            // Echo entrada

    UART0_Init();

    char buffer[32];
    int angle = 0;
    int direction = 1;
    // 2048 pasos / 180 grados ≈ 11 pasos por grado (para un 28BYJ-48)
    int steps_per_degree = 11;

    while(1) {
        // Medir distancia
        uint32_t distance = measure_distance();

        // Formatear y enviar datos: "angulo,distancia\n"
        sprintf(buffer, "%d,%lu\n", angle, distance);
        UART0_Print(buffer);

        // Mover motor equivalente a 1 grado
        move_motor(steps_per_degree, direction);

        // Actualizar ángulo
        angle += direction;

        // Invertir dirección si llegamos a los límites (0 o 180)
        if(angle >= 180) {
            direction = -1;
        } else if(angle <= 0) {
            direction = 1;
        }
    }
}
