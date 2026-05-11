#include "MKL25Z4.h"

#define RS 0x04
#define EN 0x20

volatile uint32_t pulsos    = 0;
volatile uint32_t frecuencia = 0;

void LCD_init(void);
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void LCD_print(char *text);
void LCD_nibble_write(unsigned char data, unsigned char control);
void delayMs(int n);
void delayUs(int n);
void TPM0_init(void);
void pin_frecuencia_init(void);
void iniciar_cristal(void);

int main(void) {
    iniciar_cristal();
    LCD_init();
    pin_frecuencia_init();
    TPM0_init();

    LCD_command(0x01);
    delayMs(5);

    while (1) {
        LCD_command(0x80);
        LCD_print("Freq: ");

        __disable_irq();
        uint32_t f = frecuencia;
        __enable_irq();

        LCD_data((f / 10000) % 10 + '0');
        LCD_data((f / 1000)  % 10 + '0');
        LCD_data((f / 100)   % 10 + '0');
        LCD_data((f / 10)    % 10 + '0');
        LCD_data((f)         % 10 + '0');

        LCD_print(" Hz  ");

        delayMs(200);
    }
}

void pin_frecuencia_init(void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
    PORTA->PCR[1] = (0x9 << 16) | PORT_PCR_MUX(1);
    PTA->PDDR &= ~(1u << 1);
    NVIC_EnableIRQ(PORTA_IRQn);
}

void PORTA_IRQHandler(void) {
    if (PORTA->ISFR & (1u << 1)) {
        pulsos++;
        PORTA->ISFR = (1u << 1);
    }
}

void TPM0_init(void) {
    SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;

    SIM->SOPT2 = (SIM->SOPT2 & ~(SIM_SOPT2_TPMSRC_MASK | SIM_SOPT2_PLLFLLSEL_MASK))
               | SIM_SOPT2_TPMSRC(1)
               | SIM_SOPT2_PLLFLLSEL(1);

    TPM0->SC  = 0;
    TPM0->CNT = 0;
    TPM0->MOD = 375000 - 1;
    TPM0->SC  = TPM_SC_TOIE_MASK | TPM_SC_PS(7);

    NVIC_EnableIRQ(TPM0_IRQn);

    TPM0->SC |= TPM_SC_CMOD(1);
}

void TPM0_IRQHandler(void) {
    TPM0->SC |= TPM_SC_TOF_MASK;
    frecuencia = pulsos;
    pulsos     = 0;
}

void delayMs(int n) {
    SysTick->LOAD = 48000 - 1;
    SysTick->VAL  = 0;
    SysTick->CTRL = 5;
    for (int i = 0; i < n; i++)
        while (!(SysTick->CTRL & 0x10000));
    SysTick->CTRL = 0;
}

void delayUs(int n) {
    SysTick->LOAD = 48 - 1;
    SysTick->VAL  = 0;
    SysTick->CTRL = 5;
    for (int i = 0; i < n; i++)
        while (!(SysTick->CTRL & 0x10000));
    SysTick->CTRL = 0;
}

void LCD_nibble_write(unsigned char data, unsigned char control) {
    PTD->PCOR = 0xF0;
    PTD->PSOR = (data & 0xF0);

    if (control & RS)
        PTA->PSOR = RS;
    else
        PTA->PCOR = RS;

    PTA->PSOR = EN;
    delayUs(1);
    PTA->PCOR = EN;
    delayUs(50);
}

void LCD_command(unsigned char command) {
    LCD_nibble_write(command & 0xF0, 0);
    LCD_nibble_write((command << 4) & 0xF0, 0);

    if (command < 4)
        delayMs(5);
    else
        delayUs(100);
}

void LCD_data(unsigned char data) {
    LCD_nibble_write(data & 0xF0, RS);
    LCD_nibble_write((data << 4) & 0xF0, RS);
    delayUs(100);
}

void LCD_print(char *text) {
    while (*text)
        LCD_data(*text++);
}

void LCD_init(void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;

    PORTD->PCR[4] = PORT_PCR_MUX(1);
    PORTD->PCR[5] = PORT_PCR_MUX(1);
    PORTD->PCR[6] = PORT_PCR_MUX(1);
    PORTD->PCR[7] = PORT_PCR_MUX(1);
    PTD->PDDR |= 0xF0;

    PORTA->PCR[2] = PORT_PCR_MUX(1);
    PORTA->PCR[5] = PORT_PCR_MUX(1);
    PTA->PDDR |= (RS | EN);

    delayMs(50);

    LCD_nibble_write(0x30, 0); delayMs(5);
    LCD_nibble_write(0x30, 0); delayUs(200);
    LCD_nibble_write(0x30, 0); delayUs(200);
    LCD_nibble_write(0x20, 0); delayMs(5);

    LCD_command(0x28);
    LCD_command(0x0C);
    LCD_command(0x01);
    delayMs(5);
    LCD_command(0x06);
}

void iniciar_cristal(void) {
    SIM->CLKDIV1 = SIM_CLKDIV1_OUTDIV1(1) | SIM_CLKDIV1_OUTDIV4(3);

    SIM->SCGC5   |= SIM_SCGC5_PORTA_MASK;
    PORTA->PCR[18] &= ~PORT_PCR_MUX_MASK;
    PORTA->PCR[19] &= ~PORT_PCR_MUX_MASK;

    OSC0->CR = OSC_CR_ERCLKEN_MASK;
    MCG->C2  = MCG_C2_RANGE0(1) | MCG_C2_EREFS0_MASK;

    MCG->C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(3);
    while (!(MCG->S & MCG_S_OSCINIT0_MASK));
    while (MCG->S & MCG_S_IREFST_MASK);
    while ((MCG->S & MCG_S_CLKST_MASK) != MCG_S_CLKST(2));

    MCG->C5 = MCG_C5_PRDIV0(1);
    MCG->C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV0(0);
    while (!(MCG->S & MCG_S_PLLST_MASK));
    while (!(MCG->S & MCG_S_LOCK0_MASK));

    MCG->C1 &= ~MCG_C1_CLKS_MASK;
    while ((MCG->S & MCG_S_CLKST_MASK) != MCG_S_CLKST(3));
}
