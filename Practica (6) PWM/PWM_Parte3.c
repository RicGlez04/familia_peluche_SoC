#include <MKL25Z4.h>

/* --- PROTOTIPOS DE FUNCIONES --- */
void delayMs(int n);
void delayUs(int n);

// Keypad
void keypad_init(void);
char keypad_getkey(void);

// LED
void LED_init(void);

// SERVO (PWM a 50 Hz en PTB0)
void SERVO_init(void);
void SERVO_setAngle(unsigned char angle);

// ADC (Lectura de Potenciómetro en PTE20)
void ADC0_init(void);
unsigned short ADC0_read(void);

// LCD
void LCD_command(unsigned char command);
void LCD_init(void);
void LCD_nibble_write(unsigned char data, unsigned char control);
void LCD_data(unsigned char data);
void LCD_print(char *text);

void push_botton1(void);
int boton_presionado1(void);
void push_botton2(void);
int boton_presionado2(void);

/* --- DEFINICIONES DE PINES PARA LCD --- */
#define RS 0x04 /* PTA2 */
#define EN 0x20 /* PTA5 */


/* --- FUNCIÓN PRINCIPAL --- */
int main(void)
{
    unsigned char selectedMode = 0;
    unsigned char key;
    unsigned char regresarInicio = 0;

    // Inicializamos todos los periféricos
    keypad_init();
    LED_init();
    SERVO_init();  // Inicializa el PWM para el Servo en PTB0
    ADC0_init();   // Inicializa el ADC0 en el pin PTE20 (Canal 0)
    LCD_init();
    push_botton1();
    push_botton2();

    // Mensaje inicial
    LCD_command(0x01);
    delayMs(2);
    LCD_print("Set input mode");
    LCD_command(0xC0);
    LCD_print("1:M   2:A");

    while(1) {
        // Regreso forzado al menú inicial por el Botón 2 (Paro / Reset)
        if(regresarInicio) {
            SERVO_setAngle(0); // Detiene el motor poniéndolo en 0° por seguridad
            LCD_command(0x01);
            delayMs(2);
            LCD_print("Set input mode");
            LCD_command(0xC0);
            LCD_print("1:M   2:A");
            selectedMode = 0;
            regresarInicio = 0;
            delayMs(300);       // Anti-rebote
        }

        key = keypad_getkey();

        if(key != 0) {
            // --- PASO 1: SELECCIÓN DE MODO (1 o 2) ---
            if(key == '1' || key == '2') {
                selectedMode = key;

                LCD_command(0x01);
                delayMs(2);
                LCD_command(0x80);
                LCD_print("Mode: ");
                LCD_data(selectedMode == '1' ? 'M' : 'A');

                LCD_command(0xC0);
                LCD_print("Confirmar con '#'");
            }

            // --- PASO 2: CONFIRMACIÓN CON '#' ---
            if(key == '#' && selectedMode != 0) {
                LCD_command(0x01);
                delayMs(2);

                if(selectedMode == '1') {
                    unsigned char enModoManual = 1;

                    while(enModoManual) {
                        if(boton_presionado2()){
                            regresarInicio = 1;
                            break;
                        }

                        // Menú de Selección de Velocidades Manuales
                        LCD_command(0x80);
                        LCD_print("Select Speed");
                        LCD_command(0xC0);
                        LCD_print("1:L 2:M 3:MH 4:H");

                        key = 0;
                        while(key == 0) {
                            key = keypad_getkey();

                            if(boton_presionado1()) {
                                enModoManual = 0;
                                break;
                            }
                            if(boton_presionado2()){
                                regresarInicio = 1;
                                break;
                            }
                        }

                        if(enModoManual == 0 || regresarInicio == 1) {
                            break;
                        }

                        // --- MODO MANUAL: CONTROL DEL SERVO POR PASOS ---
                        if(key >= '1' && key <= '4') {
                            LCD_command(0x01);
                            delayMs(2);
                            LCD_command(0x80);

                            unsigned char paso_grados = 1;
                            if(key == '1') { LCD_print("Vel: Lenta (25%)"); paso_grados = 1; }
                            else if(key == '2') { LCD_print("Vel: Media (50%)"); paso_grados = 2; }
                            else if(key == '3') { LCD_print("Vel: M. Rap (75%)"); paso_grados = 3; }
                            else if(key == '4') { LCD_print("Vel: Rapida(100%)"); paso_grados = 4; }

                            LCD_command(0xC0);
                            LCD_print("B1:Sig  B2:Menu");

                            delayMs(500);
                            short angulo_actual = 0;
                            char direccion_barrido = 1;

                            while(boton_presionado1() == 0) {
                                if(boton_presionado2()){
                                    regresarInicio = 1;
                                    break;
                                }

                                SERVO_setAngle((unsigned char)angulo_actual);
                                delayMs(30);

                                if (direccion_barrido == 1) {
                                    angulo_actual += paso_grados;
                                    if (angulo_actual >= 180) { angulo_actual = 180; direccion_barrido = 0; }
                                } else {
                                    angulo_actual -= paso_grados;
                                    if (angulo_actual <= 0) { angulo_actual = 0; direccion_barrido = 1; }
                                }
                            }

                            if(regresarInicio == 1) break;

                            LCD_command(0x01);
                            delayMs(2);
                            delayMs(300);
                        } else {
                            enModoManual = 0;
                        }
                    }
                }
                else if(selectedMode == '2') {
                    // --- MODO AUTOMÁTICO: MONITOREO CONTINUO DEL ADC ---
                    LCD_command(0x01);
                    delayMs(2);

                    unsigned short adc_val = 0;
                    float voltaje = 0.0;
                    unsigned char paso_grados = 1;
                    short angulo_actual = 0;
                    char direccion_barrido = 1;

                    // El ciclo se ejecuta continuamente hasta presionar Botón 1 o Botón 2
                    while(boton_presionado1() == 0) {
                        if(boton_presionado2()){
                            regresarInicio = 1;
                            break;
                        }

                        // 1. Leer el potenciómetro
                        adc_val = ADC0_read();
                        // Convertir a voltaje real (Resolución de 12 bits = 4095, Vref = 3.3V)
                        voltaje = (adc_val * 3.3f) / 4095.0f;

                        // 2. FLUJO CONDICIONAL SEGÚN LAS INSTRUCCIONES DEL LAB
                        if (voltaje >= 0.0f && voltaje <= 0.75f) {
                            LCD_command(0x80);
                            LCD_print("Mode 1: L (25%) ");
                            paso_grados = 1;
                        }
                        else if (voltaje > 0.75f && voltaje <= 1.50f) {
                            LCD_command(0x80);
                            LCD_print("Mode 2: M (50%) ");
                            paso_grados = 2;
                        }
                        else if (voltaje > 1.50f && voltaje <= 2.25f) {
                            LCD_command(0x80);
                            LCD_print("Mode 3: MH(75%) ");
                            paso_grados = 3;
                        }
                        else if (voltaje > 2.25f) { // De 2.25V hasta 3V+
                            LCD_command(0x80);
                            LCD_print("Mode 4: H (100%)");
                            paso_grados = 4;
                        }

                        LCD_command(0xC0);
                        LCD_print("B1:Menu  B2:Paro");

                        // 3. Mover físicamente el servo basado en el paso calculado por el ADC
                        SERVO_setAngle((unsigned char)angulo_actual);
                        delayMs(30); // Tiempo de estabilización mecánica

                        if (direccion_barrido == 1) {
                            angulo_actual += paso_grados;
                            if (angulo_actual >= 180) { angulo_actual = 180; direccion_barrido = 0; }
                        } else {
                            angulo_actual -= paso_grados;
                            if (angulo_actual <= 0) { angulo_actual = 0; direccion_barrido = 1; }
                        }
                    }

                    SERVO_setAngle(0); // Forzar paro del motor al salir
                }

                if(regresarInicio == 0) {
                    delayMs(1500);
                    LCD_command(0x01);
                    delayMs(2);
                    LCD_print("Set input mode");
                    LCD_command(0xC0);
                    LCD_print("1:M   2:A");
                    selectedMode = 0;
                } else {
                    selectedMode = 0;
                }
            }
            delayMs(300);
        }
    }
}

/* =========================================
 * DRIVER DEL ADC0 (LECTURA DE VOLTAJE)
 * ========================================= */

void ADC0_init(void) {
    // 1. Habilitar reloj para el módulo ADC0 y el Puerto E
    SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

    // 2. Configurar el pin PTE20 en modo analógico (Alt 0)
    PORTE->PCR[20] &= ~PORT_PCR_MUX_MASK;

    // 3. Configurar registros del ADC
    // Bus clock / 2, conversión de 12 bits (Modo largo), muestreo corto
    ADC0->CFG1 = ADC_CFG1_ADICLK(1) | ADC_CFG1_MODE(1) | ADC_CFG1_ADLSMP_MASK;
    // Seleccionar canales de entrada grupo A, software trigger
    ADC0->SC2 = 0;
}

unsigned short ADC0_read(void) {
    // Iniciar la conversión en el canal 0 (PTE20 corresponde a DADP0/ADC0_SE0)
    ADC0->SC1[0] = 0 & ADC_SC1_ADCH_MASK;

    // Esperar a que la conversión termine (COCO = Conversion Complete)
    while(!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));

    // Retornar el resultado digital de 12 bits (0 a 4095)
    return ADC0->R[0];
}

/* =========================================
 * DRIVER PARA CONTROL DE SERVOMOTOR (TPM1)
 * ========================================= */

void SERVO_init(void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
    SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
    PORTB->PCR[0] = PORT_PCR_MUX(3);
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);

    TPM1->SC = 0;
    TPM1->CONTROLS[0].CnSC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK;
    TPM1->MOD = 6552;
    TPM1->CONTROLS[0].CnV = 164; // Inicia en 0 grados reales
    TPM1->SC = TPM_SC_CMOD(1) | TPM_SC_PS(7);
}

void SERVO_setAngle(unsigned char angle) {
    if (angle > 180) angle = 180;
    // Rango calibrado de 0.5ms (164) a 2.5ms (820) para el SG90
    uint32_t cnv_value = 164 + ((angle * 656) / 180);
    TPM1->CONTROLS[0].CnV = cnv_value;
}

/* =========================================
 * FUNCIONES DEL LCD (MODO 4 BITS CORREGIDO)
 * ========================================= */

void LCD_nibble_write(unsigned char data, unsigned char control) {
    PTD->PCOR = 0xF0;
    PTD->PSOR = (data & 0xF0);
    if (control & RS) PTA->PSOR = RS; else PTA->PCOR = RS;
    PTA->PSOR = EN; delayUs(50); PTA->PCOR = EN; delayUs(50);
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

void LCD_print(char *text){
    while(*text) { LCD_data(*text++); }
}

void LCD_init(void) {
    SIM->SCGC5 |= 0x1000; SIM->SCGC5 |= 0x0200;
    PORTD->PCR[4] = 0x100; PORTD->PCR[5] = 0x100; PORTD->PCR[6] = 0x100; PORTD->PCR[7] = 0x100;
    PTD->PDDR |= 0xF0;
    PORTA->PCR[2] = 0x100; PORTA->PCR[5] = 0x100; PTA->PDDR |= 0x24;
    delayMs(50);
    LCD_command(0x30); delayMs(5); LCD_command(0x30); delayUs(200); LCD_command(0x30); delayUs(200);
    LCD_command(0x20); delayMs(5);
    LCD_command(0x28); LCD_command(0x0C); LCD_command(0x01); delayMs(5); LCD_command(0x06);
}

/* =========================================
 * FUNCIONES DEL TECLADO MATRICIAL Y LEDS
 * ========================================= */

void keypad_init(void) {
    SIM->SCGC5 |= 0x0800;
    PORTC->PCR[0] = 0x103; PORTC->PCR[1] = 0x103; PORTC->PCR[2] = 0x103; PORTC->PCR[3] = 0x103;
    PORTC->PCR[4] = 0x103; PORTC->PCR[5] = 0x103; PORTC->PCR[6] = 0x103; PORTC->PCR[7] = 0x103;
    PTC->PDDR = 0x0F;
}

char keypad_getkey(void) {
    int row, col;
    const char row_select[] = {0x01, 0x02, 0x04, 0x08};
    char key_val = 0;
    PTC->PDDR |= 0x0F; PTC->PCOR = 0x0F; delayMs(20);
    col = PTC->PDIR & 0xF0; PTC->PDDR = 0;
    if (col == 0xF0) return 0;
    for (row = 0; row < 4; row++) {
        PTC->PDDR = 0; PTC->PDDR |= row_select[row]; PTC->PCOR = row_select[row];
        delayMs(20); col = PTC->PDIR & 0xF0;
        if (col != 0xF0) break;
    }
    PTC->PDDR = 0;
    if (row == 4) return 0;
    if (col == 0xE0) key_val = row*4+ 1;
    if (col == 0xD0) key_val = row*4+ 2;
    if (col == 0xB0) key_val = row*4+ 3;
    if (col == 0x70) key_val = row*4+ 4;
    if (key_val == 0) return 0;
    const char keypad_chars[16] = {
        '1', '2', '3', 'A', '4', '5', '6', 'B',
        '7', '8', '9', 'C', '*', '0', '#', 'D'
    };
    return keypad_chars[key_val - 1];
}

void LED_init(void) {
    SIM->SCGC5 |= 0x400; SIM->SCGC5 |= 0x1000;
    PORTB->PCR[18] = 0x100; PTB->PDDR |= 0x40000; PTB->PSOR |= 0x40000;
    PORTB->PCR[19] = 0x100; PTB->PDDR |= 0x80000; PTB->PSOR |= 0x80000;
    PORTD->PCR[1] = 0x100; PTD->PDDR |= 0x02; PTD->PSOR |= 0x02;
}

/* =========================================
 * FUNCIONES DE TIEMPO (SYSTICK)
 * ========================================= */

void delayMs(int n){
    int i; SysTick->LOAD = 48000 - 1; SysTick->CTRL = 0x5;
    for(i = 0; i < n; i++) { while((SysTick->CTRL & 0x10000) == 0); }
    SysTick->CTRL = 0;
}

void delayUs(int n) {
    int i; SysTick->LOAD = 48 - 1; SysTick->CTRL = 0x5;
    for(i = 0; i < n; i++) { while((SysTick->CTRL & 0x10000) == 0); }
    SysTick->CTRL = 0;
}

void push_botton1(void){ PORTD->PCR[2] = 0x103; PTD->PDDR &= ~(1 << 2); }
int boton_presionado1(void){ return ((PTD->PDIR & (1 << 2)) == 0); }

void push_botton2(void){ PORTD->PCR[3] = 0x103; PTD->PDDR &= ~(1 << 3); }
int boton_presionado2(void){ return ((PTD->PDIR & (1 << 3)) == 0); }
