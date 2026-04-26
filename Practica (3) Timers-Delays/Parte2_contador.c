
#include <MKL25Z4.h>

/* Suponiendo que conectaste: RS en PTA1, RW en PTA2, EN en PTA4 */
#define RS (1 << 1)  /* 0x02 */
#define RW (1 << 2)  /* 0x04 */
#define EN (1 << 4)  /* 0x10 */


/* LCD Part*/
void LCD_command (unsigned char command);
void LCD_data (unsigned char data);
void LCD_init(void);
void LCD_puts(char* str);

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
    
    PORTB ->PCR[19]= 0x100; /*make PTB19 pin as GPIO*/
    PTB -> PDDR |= 0x080000; /*make PTB19 as output pin */

    /*Timer (para el delay y el conteo)*/
    SIM -> SCGC6 |= 0x01000000; 
    SIM -> SOPT2 |= 0x01000000;
    TPM0 -> SC = 0; 
    TPM0 -> SC |= 0x07; /* Prescaler 128 */
    TPM0 -> MOD = 328;
    TPM0 -> SC |= 0x80; /*clear TOF*/
    TPM0 -> SC |= 0x08;


/*-----------------PROGRAMA--------------------*/
    LCD_init();

/*-------------PASO 1 -------------------------------------*/
    LCD_command(1); /*Clear display*/
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

    //Mapa para convertir el valor retornado por su funcion al numero real
    char mapa_teclado[]= {0, '1', '2', '3', 'A', '4', '5', '6', 'B', '7', '8', '9', 'C', '*', '0', '#', 'D'};

    while(1){
    key= keypad_getkey();
    if (key != 0){
        char tecla = mapa_teclado[key]; //Obtener el caracter real

        if(tecla >= '0' && tecla <= '9'){
            if (cuenta_digitos < 2){
                LCD_data(tecla);

                valor_numerico = tecla - '0';
                total_segundos = (total_segundos * 10) + valor_numerico;
                cuenta_digitos++;

                delayMs(300);

            }
        }
        else if ((tecla == '*' || tecla == '#' ) && cuenta_digitos > 0 ) {
            break;
        }
    }
}

/*--------------------Contador ascendente----------------------*/
    LCD_command(1);
    delayMs(10);
    LCD_puts("Contando...");

    short int U, D ;

    for( int i =0 ; i<= total_segundos; i++){
        LCD_command(0xC0);

        D = i / 10; //Unidades
        U = i % 10; //Decenas 

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
    for(i = 0; i < n; i++) {
        while((TPM0->SC & 0x80) == 0) { } // Espera el flag TOF
        TPM0->SC |= 0x80;                 // Limpia el flag TOF
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
    PTA->PSOR = RS; 
    PTA->PCOR = RW;
    PTD->PDOR = data;
    PTA->PSOR = EN; 
    delayMs(1);    // Pequeño pulso
    PTA->PCOR = EN;
    delayMs(1);
}

void LCD_command(unsigned char command)
{
	PTA->PCOR = RS | RW; /* RS = 0, R/W = 0 */
	PTD->PDOR = command;
	PTA->PSOR = EN; /* pulse E */

	delayMs(0);
	PTA->PCOR = EN;

	if (command < 4)
		delayMs(4); /* command 1 and 2 needs up to 1.64ms */
	else
		delayMs(1); /* all others 40 us */
}


void LCD_init(void)
{
	SIM->SCGC5 |= 0x1000; /* enable clock to Port D */
	PORTD->PCR[0] = 0x100; /* make PTD0 pin as GPIO */
	PORTD->PCR[1] = 0x100; /* make PTD1 pin as GPIO */
	PORTD->PCR[2] = 0x100; /* make PTD1 pin as GPIO */
	PORTD->PCR[3] = 0x100; /* make PTD1 pin as GPIO */
	PORTD->PCR[4] = 0x100; /* make PTD1 pin as GPIO */
	PORTD->PCR[5] = 0x100; /* make PTD1 pin as GPIO */
	PORTD->PCR[6] = 0x100; /* make PTD6 pin as GPIO */
	PORTD->PCR[7] = 0x100; /* make PTD7 pin as GPIO */
	PTD->PDDR = 0xFF; /* make PTD7-0 as output pins */
	SIM->SCGC5 |= 0x0200; /* enable clock to Port A */
	PORTA->PCR[2] = 0x100; /* make PTA2 pin as GPIO */
	PORTA->PCR[4] = 0x100; /* make PTA4 pin as GPIO */
	PORTA->PCR[5] = 0x100; /* make PTA5 pin as GPIO */
	PTA->PDDR |= 0x34; /* make PTA5, 4, 2 as out pins*/

	delayMs(30); /* initialization sequence */

	LCD_command(0x30);
	delayMs(10);
	LCD_command(0x30);
	delayMs(1);
	LCD_command(0x30);
	/* set 8-bit data, 2-line, 5x7 font */
	LCD_command(0x38);
	/* move cursor right */
	LCD_command(0x06);
	/* clear screen, move cursor to home */
	LCD_command(0x01);
	/* turn on display, cursor blinking */
	LCD_command(0x0F);}
      

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
	/* one row is active */
	/* check to see any key pressed */

	PTC->PDDR |= 0x0F; /* enable all rows */
	PTC->PCOR = 0x0F;
	delayUs(2); /* wait for signal return */
	col = PTC-> PDIR & 0xF0; /* read all columns */
	PTC->PDDR = 0; /* disable all rows */
	if (col == 0xF0)
		return 0; /* no key pressed */

	/* If a key is pressed, we need find out which key.*/
	for (row = 0; row < 4; row++)
	{ PTC->PDDR = 0; /* disable all rows */

	PTC->PDDR |= row_select[row]; /* enable one row */
	PTC->PCOR = row_select[row]; /* drive active row low*/

	delayUs(2); /* wait for signal to settle */
	col = PTC->PDIR & 0xF0; /* read all columns */

	if (col != 0xF0) break;
	/* if one of the input is low, some key is pressed. */
	}

	PTC->PDDR = 0; /* disable all rows */

	if (row == 4)
		return 0; /* if we get here, no key is pressed */

	/* gets here when one of the rows has key pressed*/
	/*check which column it is*/

	if (col == 0xE0) return row*4+ 1; /* key in column 0 */
	if (col == 0xD0) return row*4+ 2; /* key in column 1 */
	if (col == 0xB0) return row*4+ 3; /* key in column 2 */
	if (col == 0x70) return row*4+ 4; /* key in column 3 */
	return 0; /* just to be safe */
}
