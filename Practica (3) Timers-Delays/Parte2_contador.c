
#include <MKL25Z4.h>

/* Suponiendo que conectaste: RS en PTA1, RW en PTA2, EN en PTA4 */
#define RS 0x04 /* PTA2 */
#define EN 0x20 /* PTA5 */

/* LCD Part*/
void LCD_command (unsigned char command);
void LCD_data (unsigned char data);
void LCD_init(void);
void LCD_puts(char* str);
void LCD_nibble_write(unsigned char data, unsigned char control);

/*Timer part*/
  void delayMs(int n);

/*Keypad part*/
void keypad_init(void);
char keypad_getkey(void);
void delayUs (int n);


int main(void){
    /*Iniciar*/
    SIM -> SCGC5 |= 0x400;  /* enable clock to port B */
    SIM -> SCGC5 |= 0X200;  /* enable clock to port A */
    SIM -> SCGC5 |= 0X1000; /* enable clock to port D */

    PORTB->PCR[19] = 0x100;      // PTB19 como GPIO
    PTB->PDDR |= (1 << 19);      // Configurar como salida
    PTB->PSOR = (1 << 19);       // APAGAR el LED (Lógica negativa: PSOR pone a 1)

    /*Timer (para el delay y el conteo)*/
    	SIM -> SCGC6 |= 0x01000000;
        SIM -> SOPT2 |= 0x01000000;
        TPM0 -> SC = 0;
        TPM0 -> SC |= 0x07; /* Prescaler 128 */
        TPM0 -> MOD = 163;  /* CAMBIO: 163 en lugar de 375 */
        TPM0 -> SC |= 0x80; /* clear TOF */
        TPM0 -> SC |= 0x08; /* Arrancar */


/*-----------------PROGRAMA--------------------*/
    keypad_init();
    LCD_init();
/*-------------PASO 1 -------------------------------------*/
    LCD_command(0x01); /*Clear display*/
    delayMs(10);
    LCD_command(0x80);/*Cursor at first of the line*/
    LCD_data('H');
	LCD_data('e');
	LCD_data('l');
	LCD_data('l');
	LCD_data('o');
    delayMs(5000); /* Delay de 5s*/


/*-------------PASO 2---------------------------------------------*/
    LCD_command(1); /*Limpiamos el display*/
    delayMs(100);
    LCD_command(0x80);
    LCD_puts("Secs: "); /* Pedir los datos al usuario*/

    int total_segundos= 0;
    int cuenta_digitos = 0;
    int valor_numerico = 0;
    unsigned char key;


    while(1){
    key= keypad_getkey();//Obtener el caracter real
        if(key >= '0' && key <= '9'){
        	LCD_data(key);
        	delayMs(30);
            if (cuenta_digitos < 2){


                valor_numerico = key - '0';
                total_segundos = (total_segundos * 10) + valor_numerico;
                cuenta_digitos++;

                delayMs(300);

            }
        }
        else if ((key == '*' || key == '#' ) && cuenta_digitos > 0 ) {
            break;
        }
    }


/*--------------------Contador ascendente----------------------*/
    LCD_command(1);
    delayMs(10);
    LCD_puts("Contando...");

    short int U, D ;

    for( int i =0 ; i<= total_segundos; i++){
        LCD_command(0xC0);

        U = i % 10; //Unidades
        D = i / 10; //Decenas

        LCD_data(D + '0');
        LCD_data(U + '0');

        delayMs(1000);

    }

    LCD_command(1);
    LCD_puts("Lo logramos");

    // Encendemos el LED verde (PTB19) para marcar que terminó
    PTB->PCOR = 0x080000; // En la KL25Z, PCOR enciende el LED (lógica negativa)
    delayMs(1000);
    PTB -> PSOR = 0x080000;// Apagar el led

    while(1){} //se queda aqui para siempre evitado que el micro se reinicie
}


/*-----------------------------------FUNCIONES----------------------------*/
void LCD_puts(char* str) {
    while(*str) {
        LCD_data(*str++);
    }
}

void delayMs(int n) {
    int i;
    TPM0->CNT = 0; // Resetear la cuenta inicial para evitar basura
    for(i = 0; i < n; i++) {
        while((TPM0->SC & 0x80) == 0) {
            // Esperar el flag TOF
        }
        TPM0->SC |= 0x80; // Limpiar el flag TOF escribiendo un 1
    }
}

void delayUs(int n) {
    int i, j;
    for(i = 0; i < n; i++) {
        for(j = 0; j < 5; j++) {
            __asm("nop"); // Pequeña espera de microsegundos
        }
    }
}

void LCD_data(unsigned char data) {
    // Misma lógica del comando, pero con la bandera RS encendida
    LCD_nibble_write(data & 0xF0, RS);
    LCD_nibble_write((data << 4) & 0xF0, RS);
    delayUs(100);
}


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


/*-----------------key pad----------------------*/
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
