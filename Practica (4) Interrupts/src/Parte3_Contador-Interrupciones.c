#include <MKL25Z4.h>

/* --- DEFINICIONES DE PINES PARA LCD --- */
#define RS 0x04 /* PTA2 */
#define EN 0x20 /* PTA5 */

/* LCD Part*/
void LCD_command (unsigned char command);
void LCD_data (unsigned char data);
void LCD_init(void);
void LCD_puts(char* str);
void LCD_nibble_write(unsigned char data, unsigned char control);

/* Timer part (Software based para reloj lento) */
void delayMs(int n);
void delayUs(int n);

/* Keypad part (Restaurada a tu versión original) */
void keypad_init(void);
char keypad_getkey(void);

volatile int paused = 0;

int main(void) {
    /* 1. Iniciar Relojes de Puertos */
    SIM->SCGC5 |= 0x400;  /* Port B */
    SIM->SCGC5 |= 0x200;  /* Port A */
    SIM->SCGC5 |= 0x1000; /* Port D */
    SIM->SCGC5 |= 0x0800; /* Port C (Keypad) */

    /* 2. Interrupción en PTA1 (Switch) */
    PORTA->PCR[1] = 0x00100 | 0x00003 | 0xA0000; // GPIO + pull-up + interrupción falling edge
    PTA->PDDR &= ~(1 << 1);
    NVIC->ISER[0] |= 0x40000000; // habilitar interrupción PORTA

    /* 3. Inicialización de Periféricos */
    keypad_init();
    LCD_init();

    __enable_irq();

    int contador = 0;
    char key;

    while(1) {
        if(paused == 0) {
            LCD_command(0x80); // Posición inicial
            LCD_puts("Cont: ");

            // Mostrar número (Unidades y Decenas)
            LCD_data((contador / 10) + '0');
            LCD_data((contador % 10) + '0');

            delayMs(800);
            contador++;
            if(contador > 99) contador = 0;
        }

        key = keypad_getkey();
        if(key == '*') {
            paused = 0;
            LCD_command(0x01); // Limpiar
            delayMs(5);
        }
    }
}

/* =========================================
 * MANEJO DE INTERRUPCIÓN
 * ========================================= */
void PORTA_IRQHandler(void) {
    paused = 1;
    LCD_command(0x01);
    delayMs(5);
    LCD_puts("PAUSED");
    PORTA->ISFR = 0x02; // IMPORTANTE: Limpiar bandera de interrupción en PTA1
}

/* =========================================
 * FUNCIONES DEL LCD (MODO 4 BITS)
 * ========================================= */
void LCD_nibble_write(unsigned char data, unsigned char control) {
    PTD->PCOR = 0xF0;
    PTD->PSOR = (data & 0xF0);

    if (control & RS)
        PTA->PSOR = RS;
    else
        PTA->PCOR = RS;

    PTA->PSOR = EN;
    delayUs(50);
    PTA->PCOR = EN;
    delayUs(50);
}

void LCD_command(unsigned char command) {
    LCD_nibble_write(command & 0xF0, 0);
    LCD_nibble_write((command << 4) & 0xF0, 0);
    if (command < 4) delayMs(5); else delayUs(100);
}

void LCD_data(unsigned char data) {
    LCD_nibble_write(data & 0xF0, RS);
    LCD_nibble_write((data << 4) & 0xF0, RS);
    delayUs(100);
}

void LCD_puts(char* str) {
    while(*str) LCD_data(*str++);
}

void LCD_init(void) {
    PORTD->PCR[4] = 0x100; PORTD->PCR[5] = 0x100;
    PORTD->PCR[6] = 0x100; PORTD->PCR[7] = 0x100;
    PTD->PDDR |= 0xF0;

    PORTA->PCR[2] = 0x100; PORTA->PCR[5] = 0x100;
    PTA->PDDR |= (RS | EN);

    delayMs(50);

    // Inicialización manual modo 4 bits
    LCD_nibble_write(0x30, 0);
    delayMs(5);
    LCD_nibble_write(0x30, 0);
    delayUs(200);
    LCD_nibble_write(0x30, 0);
    delayUs(200);
    LCD_nibble_write(0x20, 0);
    delayMs(5);

    LCD_command(0x28); LCD_command(0x0C);
    LCD_command(0x01); delayMs(5);
    LCD_command(0x06);
}

/* =========================================
 * FUNCIONES DE TIEMPO (RELOJ LENTO ~21MHz)
 * ========================================= */
void delayMs(int n) {
    int i, j;
    for(i = 0; i < n; i++) {
        for(j = 0; j < 3500; j++) { __asm("nop"); }
    }
}

void delayUs(int n) {
    int i, j;
    for(i = 0; i < n; i++) {
        for(j = 0; j < 3; j++) { __asm("nop"); }
    }
}

/* =========================================
 * KEYPAD (TU VERSIÓN ORIGINAL)
 * ========================================= */
void keypad_init(void) {
    SIM->SCGC5 |= 0x0800;
    PORTC->PCR[0] = 0x103; PORTC->PCR[1] = 0x103;
    PORTC->PCR[2] = 0x103; PORTC->PCR[3] = 0x103;
    PORTC->PCR[4] = 0x103; PORTC->PCR[5] = 0x103;
    PORTC->PCR[6] = 0x103; PORTC->PCR[7] = 0x103;
    PTC->PDDR = 0x0F;
}

char keypad_getkey(void) {
    int row, col;
    const char row_select[] = {0x01, 0x02, 0x04, 0x08};
    char key_val = 0;

    PTC->PDDR |= 0x0F;
    PTC->PCOR = 0x0F;
    delayMs(20);
    col = PTC->PDIR & 0xF0;
    PTC->PDDR = 0;

    if (col == 0xF0) return 0;

    for (row = 0; row < 4; row++) {
        PTC->PDDR = 0;
        PTC->PDDR |= row_select[row];
        PTC->PCOR = row_select[row];
        delayMs(20);
        col = PTC->PDIR & 0xF0;
        if (col != 0xF0) break;
    }
    PTC->PDDR = 0;
    if (row == 4) return 0;

    if (col == 0xE0) key_val = row*4+ 1;
    if (col == 0xD0) key_val = row*4+ 2;
    if (col == 0xB0) key_val = row*4+ 3;
    if (col == 0x70) key_val = row*4+ 4;

    const char keypad_chars[16] = {
        '1', '2', '3', 'A', '4', '5', '6', 'B',
        '7', '8', '9', 'C', '*', '0', '#', 'D'
    };
    return keypad_chars[key_val - 1];
}
