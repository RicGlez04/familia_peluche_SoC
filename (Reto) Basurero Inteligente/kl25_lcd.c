#include <MKL25Z4.h>

// --- DEFINICIONES DE PINES ---
#define RS (1 << 4) // Puerto A, Pin 4
#define EN (1 << 5) // Puerto A, Pin 5
#define PIR_PIN (1 << 8) // Puerto C, Pin 8
#define BOTON_ORG (1 << 7) // PTC7
#define BOTON_INORG (1 << 6) // PTC6

// Pines de LEDs (Puerto B)
#define LED_AZUL  (1 << 8)  // PTB8
#define LED_AMAR  (1 << 9)  // PTB9
#define LED_VERDE (1 << 10) // PTB10
#define LED_ROJO  (1 << 11) // PTB11

// --- PROTOTIPOS ---
void delayMs(int n);
void delayUs(int n);
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void LCD_init(void);
void init_GPIO(void);
void LCD_clear_and_print(char *line1, char *line2);
void UART0_init(void);
void UART0_send(char c);
void set_LEDs(int azul, int amar, int verde, int rojo);

// --- IMPLEMENTACION ---
void delayMs(int n) { volatile int i, j; for (i = 0; i < n; i++) for (j = 0; j < 7000; j++) {} }
void delayUs(int n) { volatile int i; for (i = 0; i < n * 7; i++) {} }

void LCD_nibble_write(unsigned char data, unsigned char control) {
    PTD->PDOR = (PTD->PDOR & 0x0F) | (data & 0xF0);
    if (control & RS) PTA->PSOR = RS; else PTA->PCOR = RS;
    PTA->PSOR = EN; delayUs(1); PTA->PCOR = EN;
    delayUs(100);
}

void LCD_command(unsigned char command) { LCD_nibble_write(command & 0xF0, 0); LCD_nibble_write((command << 4) & 0xF0, 0); }
void LCD_data(unsigned char data) { LCD_nibble_write(data & 0xF0, RS); LCD_nibble_write((data << 4) & 0xF0, RS); }

void init_GPIO(void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK;
    PORTD->PCR[4] = PORTD->PCR[5] = PORTD->PCR[6] = PORTD->PCR[7] = 0x100; PTD->PDDR |= 0xF0;
    PORTA->PCR[4] = PORTA->PCR[5] = 0x100; PTA->PDDR |= RS | EN;
    PORTB->PCR[8] = PORTB->PCR[9] = PORTB->PCR[10] = PORTB->PCR[11] = 0x100; PTB->PDDR |= LED_AZUL | LED_AMAR | LED_VERDE | LED_ROJO;
    PORTC->PCR[8] = 0x100;
    PORTC->PCR[7] = 0x103; PORTC->PCR[6] = 0x103;
    PTC->PDDR &= ~(PIR_PIN | BOTON_ORG | BOTON_INORG);
}

void set_LEDs(int azul, int amar, int verde, int rojo) {
    PTB->PCOR = LED_AZUL | LED_AMAR | LED_VERDE | LED_ROJO;
    if (azul) PTB->PSOR = LED_AZUL;
    if (amar) PTB->PSOR = LED_AMAR;
    if (verde) PTB->PSOR = LED_VERDE;
    if (rojo) PTB->PSOR = LED_ROJO;
}

void LCD_init(void) {
    delayMs(30); LCD_command(0x30); delayMs(5);
    LCD_command(0x30); delayUs(200); LCD_command(0x30);
    LCD_command(0x20); LCD_command(0x28); LCD_command(0x0C);
    LCD_command(0x01); delayMs(2); LCD_command(0x06);
}

void LCD_print(char *str) { while (*str) LCD_data(*str++); }
void LCD_clear_and_print(char *line1, char *line2) { LCD_command(0x01); delayMs(2); LCD_print(line1); LCD_command(0xC0); LCD_print(line2); }

void UART0_init(void) {
    SIM->SCGC4 |= SIM_SCGC4_UART0_MASK; SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
    PORTA->PCR[1] = PORT_PCR_MUX(2); PORTA->PCR[2] = PORT_PCR_MUX(2);
    SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1);
    UART0->C4 = UART0_C4_OSR(15);
    UART0->BDH = 0x00; UART0->BDL = 0x88;
    UART0->C2 = UART0_C2_RE_MASK | UART0_C2_TE_MASK;
}

void UART0_send(char c) { while (!(UART0->S1 & UART0_S1_TDRE_MASK)); UART0->D = c; }

int main(void) {
    init_GPIO(); UART0_init(); LCD_init();
    set_LEDs(0, 0, 1, 0); // Verde inicial
    LCD_clear_and_print("Sistema Listo", "Esperando sensor");

    int pir_disparado = 0;

    while(1) {
        if ((PTC->PDIR & PIR_PIN) && (pir_disparado == 0)) {
            UART0_send('S');
            pir_disparado = 1;
        }
        if (!(PTC->PDIR & PIR_PIN)) pir_disparado = 0;

        if (UART0->S1 & UART_S1_RDRF_MASK) {
            char cmd = UART0->D;
            switch(cmd) {
                case 'A':
                    set_LEDs(1, 0, 0, 0);
                    LCD_clear_and_print("Procesando...", "Capturando...");
                    break;
                case 'O': // Nuevo: Orgánico detectado
                    LCD_clear_and_print("Organico", "Detectado");
                    delayMs(3000);
                    set_LEDs(0, 0, 1, 0); LCD_clear_and_print("Sistema Listo", "Esperando sensor");
                    break;
                case 'I': // Nuevo: Inorgánico detectado
                    LCD_clear_and_print("Inorganico", "Detectado");
                    delayMs(3000);
                    set_LEDs(0, 0, 1, 0); LCD_clear_and_print("Sistema Listo", "Esperando sensor");
                    break;
                case 'U':
                    set_LEDs(0, 1, 0, 0);
                    LCD_clear_and_print("Incertidumbre", "Presione boton");
                    while(1) {
                        if (!(PTC->PDIR & BOTON_ORG)) { UART0_send('D'); break; }
                        if (!(PTC->PDIR & BOTON_INORG)) { UART0_send('I'); break; }
                    }
                    set_LEDs(0, 0, 1, 0); LCD_clear_and_print("Sistema Listo", "Esperando sensor");
                    break;
                case 'E':
                    set_LEDs(0, 0, 0, 1);
                    LCD_clear_and_print("Nada detectado", "Esperando sensor");
                    break;
                case 'R':
                    set_LEDs(0, 0, 1, 0);
                    LCD_clear_and_print("Sistema Listo", "Esperando sensor");
                    pir_disparado = 0;
                    break;
            }
        }
    }
}
