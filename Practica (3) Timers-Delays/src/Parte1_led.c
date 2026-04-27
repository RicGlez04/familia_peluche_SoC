#include <MKL25Z4.h>

/* --- PROTOTIPOS DE FUNCIONES --- */
void delayMs(int n);
void delayUs(int n);

// Keypad
void keypad_init(void);
char keypad_getkey(void);

// LED
void LED_init(void);
void RGB_setColor(char key);

// LCD
void LCD_command(unsigned char command);
void LCD_init(void);
void LCD_nibble_write(unsigned char data, unsigned char control);
void LCD_data(unsigned char data);
void LCD_print(char *text);

/* --- DEFINICIONES DE PINES PARA LCD --- */
#define RS 0x04 /* PTA2 */
#define EN 0x20 /* PTA5 */


/* --- FUNCIÓN PRINCIPAL --- */
int main(void)
{
	unsigned char selectedColor;
    unsigned char key;

    // Inicializamos todos los periféricos
    keypad_init();
    LED_init();
    LCD_init();

    // Imprimir un mensaje de prueba inicial
    LCD_command(0x80); // Ir a la primera línea, primera posición

    // Bucle principal (Lectura del teclado y LEDs)
    while(1){
            key = keypad_getkey();

            if(key != 0){

                // Si se presiona un numero valido
                if(key >= '1' && key <= '8'){
                    selectedColor = key;

                    LCD_command(0x01);
                    delayMs(2);

                    LCD_command(0x80);
                    LCD_print("Color sel:");

                    LCD_command(0xC0);

                    switch(selectedColor){
                        case '1':
                            LCD_print("1 Rojo");
                            break;

                        case '2':
                            LCD_print("2 Verde");
                            break;

                        case '3':
                            LCD_print("3 Azul");
                            break;

                        case '4':
                            LCD_print("4 Amarillo");
                            break;

                        case '5':
                            LCD_print("5 Morado");
                            break;

                        case '6':
                            LCD_print("6 Cian");
                            break;

                        case '7':
                            LCD_print("7 Blanco");
                            break;

                        case '8':
                            LCD_print("8 Apagar");
                            break;
                    }
                }

                // Confirmar con *
                if(key == '*' && selectedColor != 0){
                    RGB_setColor(selectedColor);

                    LCD_command(0x01);
                    delayMs(2);

                    LCD_command(0x80);
                    LCD_print("Color aplicado");

                    LCD_command(0xC0);

                    switch(selectedColor){
                        case '1':
                            LCD_print("Rojo");
                            break;

                        case '2':
                            LCD_print("Verde");
                            break;

                        case '3':
                            LCD_print("Azul");
                            break;

                        case '4':
                            LCD_print("Amarillo");
                            break;

                        case '5':
                            LCD_print("Morado");
                            break;

                        case '6':
                            LCD_print("Cian");
                            break;

                        case '7':
                            LCD_print("Blanco");
                            break;

                        case '8':
                            LCD_print("Apagado");
                            break;
                    }

                    delayMs(500);

                    LCD_command(0x01);
                    delayMs(2);

                    LCD_command(0x80);
                    LCD_print("Hello");

                    LCD_command(0xC0);
                    LCD_print("1-8 y luego *");
                }

                delayMs(250);// aguas con este porque si se le mueve salen geroglificos raros
            }
        }
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


/* =========================================
 * FUNCIONES DEL TECLADO MATRICIAL Y LEDS
 * ========================================= */

void keypad_init(void)
{
    SIM->SCGC5 |= 0x0800;  /* enable clock to Port C */
    PORTC->PCR[0] = 0x103; /* PTD0, GPIO, enable pullup*/
    PORTC->PCR[1] = 0x103; /* PTD1, GPIO, enable pullup*/
    PORTC->PCR[2] = 0x103; /* PTD2, GPIO, enable pullup*/
    PORTC->PCR[3] = 0x103; /* PTD3, GPIO, enable pullup*/
    PORTC->PCR[4] = 0x103; /* PTD4, GPIO, enable pullup*/
    PORTC->PCR[5] = 0x103; /* PTD5, GPIO, enable pullup*/
    PORTC->PCR[6] = 0x103; /* PTD6, GPIO, enable pullup*/
    PORTC->PCR[7] = 0x103; /* PTD7, GPIO, enable pullup*/
    PTC->PDDR = 0x0F; /* make PTD7-0 as input pins */
}

char keypad_getkey(void) {
	int row, col;
	    const char row_select[] = {0x01, 0x02, 0x04, 0x08};
	    char key_val = 0; // Guardaremos el valor numérico aquí

	    PTC->PDDR |= 0x0F;
	    PTC->PCOR = 0x0F;
	    delayMs(20);
	    col = PTC-> PDIR & 0xF0;
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

	    // Obtener la posición matemática (1 a 16)
	    if (col == 0xE0) key_val = row*4+ 1;
	    if (col == 0xD0) key_val = row*4+ 2;
	    if (col == 0xB0) key_val = row*4+ 3;
	    if (col == 0x70) key_val = row*4+ 4;

	    if (key_val == 0) return 0;

	    // Mapa de caracteres de un teclado matricial 4x4 estándar
	    const char keypad_chars[16] = {
	        '1', '2', '3', 'A',
	        '4', '5', '6', 'B',
	        '7', '8', '9', 'C',
	        '*', '0', '#', 'D'
	    };

	    // Devolvemos el carácter ASCII exacto para que tu main funcione
	    return keypad_chars[key_val - 1];
    }

void LED_init(void)
{
    SIM->SCGC5 |= 0x400; /* enable clock to Port B */
    SIM->SCGC5 |= 0x1000; /* enable clock to Port D */
    PORTB->PCR[18] = 0x100; /* make PTB18 pin as GPIO */
    PTB->PDDR |= 0x40000; /* make PTB18 as output pin */
    PTB->PSOR |= 0x40000; /* turn off red LED */
    PORTB->PCR[19] = 0x100; /* make PTB19 pin as GPIO */
    PTB->PDDR |= 0x80000; /* make PTB19 as output pin */
    PTB->PSOR |= 0x80000; /* turn off green LED */
    PORTD->PCR[1] = 0x100; /* make PTD1 pin as GPIO */
    PTD->PDDR |= 0x02; /* make PTD1 as output pin */
    PTD->PSOR |= 0x02; /* turn off blue LED */
}


void RGB_off(void){
    // Turn off all colors (active low)
    PTB->PSOR = (1 << 18);
    PTB->PSOR = (1 << 19);
    PTD->PSOR = (1 << 1);
}


void RGB_setColor(char key){
    RGB_off(); // Clear previous color

    // Select color based on key
    switch(key){
        case '1': // Red
            PTB->PCOR = (1 << 18);
            break;

        case '2': // Green
            PTB->PCOR = (1 << 19);
            break;

        case '3': // Blue
            PTD->PCOR = (1 << 1);
            break;

        case '4': // Yellow (Red + Green)
            PTB->PCOR = (1 << 18);
            PTB->PCOR = (1 << 19);
            break;

        case '5': // Purple (Red + Blue)
            PTB->PCOR = (1 << 18);
            PTD->PCOR = (1 << 1);
            break;

        case '6': // Cyan (Green + Blue)
            PTB->PCOR = (1 << 19);
            PTD->PCOR = (1 << 1);
            break;

        case '7': // White (RGB)
            PTB->PCOR = (1 << 18);
            PTB->PCOR = (1 << 19);
            PTD->PCOR = (1 << 1);
            break;

        case '8': // Turn off
            RGB_off();
            break;
    }
}

/* =========================================
 * FUNCIONES DE TIEMPO (SYSTICK)
 * ========================================= */

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
