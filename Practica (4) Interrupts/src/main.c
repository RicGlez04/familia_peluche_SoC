// Práctica 4: Interrupción de un contador

/* Al presionar la tecla '*' del teclado, se . */
/* Main program toggles red LED while waiting for an interrupt from switches. */

#include <MKL25Z4.H>

// Inicialización de timer TPM0
void TPM0_init(void);

// Inicialización de PTA1
void PTA1_init();

// Funciones de delay
void delayMs(int n);

// Inicialización de la keypad
void keypad_init();

void keypad_getkey();

// Funciones para la pantalla LCD
void LCD_command(unsigned char command);
void LCD_init(void);
void LCD_nibble_write(unsigned char data, unsigned char control);
void LCD_data(unsigned char data);
void LCD_print(char *text); // Imprimir un string

// Definiciones de pines para LCD
#define RS 0x04 /* PTA2 */
#define EN 0x20 /* PTA5 */

// Definición de contador
volatile int contador = 0;

volatile int paused = 0;


// Programa principal
int main(void) {

	// Defincion de variables para la cuenta
	short int contador_de, contador_un;
	int cuenta_total = 99;

	TPM0_init(); // Inicializar registros temporizador

	__disable_irq(); /* disable all IRQs */

	unsigned char key; // Variable para guardar la tecla presionada

	// Inicialización de periféricos
	keypad_init();
	LCD_init();

	// Configurar interrupcion con pin PTA1
	PTA1_init();
	NVIC->ISER[0] |= 0x40000000; /* enable INT30 (bit 30 of ISER[0]) */
	__enable_irq(); /* global enable IRQs */

	// Limpiar pantalla LCD antes de empezar a desplegar
	LCD_command(0x01);
	delayMs(2);

	// Bucle principal
	while(1)
	{
		// Instrucciones en caso de NO ESTAR pausado
		if (paused == 0) {
			LCD_command(0x80); // Posicionar cursos en la primera linea
			LCD_print("Cuenta (0-99):"); // Desplegar texto
			LCD_command(0xC0); // Mover cursor a la segunda linea
			delayMs(2);

			contador_un = contador % 10; // Extraer unidades
			contador_de = contador / 10; // Extraer decenas

			LCD_data("0" + contador_de); // Desplegar decenas de la cuenta
			LCD_data('0' + contador_un); // Desplegar unidades de la cuenta

			delayMs(1000);
			contador++;

			// Reiniciar contador en caso de que llegue al maximo
			if (contador > cuenta_total) {
				contador = 0;
			}
		}

		// Instrucciones en caso de ESTAR pausado
		else {
			key = keypad_getkey();

			if (key == '*') {
			paused == 0; // Reanudar la cuenta
			LCD_command(0x01); // Limpiar la pantalla
			delayMs(2); // Pequeño retardo
			}
		}
	}
}


// =============== DEFINICIÓN DE FUNCIONES ===============

/* Puede ser un pushbutton o un jumper que conecte a tierra para ejectuar la
 * interrupción. */
void PORTA_IRQHandler(void) {
	// Activar pausado
	paused = 1;

	// Limpiar pantalla y desplegar 'PAUSED'
	LCD_command(0x01); // Limpiar pantalla
	delayUs(2000);	   // Pequeño delay (sin usar ms porque utiliza otra interrupcion)
	LCD_command(0x80); // Posicionar cursor en la primera línea
	LCD_print("     PAUSED     ");

	PORTA->ISFR = 0x00000006; /* clear interrupt flag */
}

// Función para inicializar timer TPM0
void TPM0_init(void) {
	/* Init code for timer*/
	SIM->SCGC6 |= 0x01000000; /* enable clock to TPM0 */
	SIM->SOPT2 |= 0x01000000; /* use 32.76khz as clock */
	TPM0->SC = 0; /* disable timer while configuring */
	TPM0->SC = 0x02; /* prescaler /4 */
	TPM0->MOD = 0x2000; /* max modulo value 8192*/
	TPM0->SC |= 0x80; /* clear TOF */
	TPM0->SC |= 0x08; /* enable timer free-running mode */
}

// Función para inicializar pin PTA1
void PTA1_init() {
	PORTA->PCR[1] |= 0x00100; /* make it GPIO */
	PORTA->PCR[1] |= 0x00003; /* enable pull-up */
	PTA->PDDR &= ~0x0002; /* make pin input */
	PORTA->PCR[1] &= ~0xF0000; /* clear interrupt selection */
	PORTA->PCR[1] |= 0xA0000; /* enable falling edge INT */
}

/* Initializes PortC that is connected to the keypad.
  Pins as GPIO input pin with pullup enabled.*/
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

	char characters[16] = {'1', '2', '3', 'A',
						   '4', '5', '6', 'B',
						   '7', '8', '9', 'C',
						   '*', '0', '#', 'D'};

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
		return ' '; /* if we get here, no key is pressed */

	/* gets here when one of the rows has key pressed*/
	/*check which column it is*/

	if (col == 0xE0) return characters[row*4+ 1]; /* key in column 0 */
	if (col == 0xD0) return characters[row*4+ 2]; /* key in column 1 */
	if (col == 0xB0) return characters[row*4+ 3]; /* key in column 2 */
	if (col == 0x70) return characters[row*4+ 4]; /* key in column 3 */
	return ' '; /* just to be safe */
}

/* Delay function */
void delayMs(int n) {
	for(int i = 0; i < n; i++)
	{
		while((TPM0->SC & 0x80) == 0) { }
		/* wait until the TOF is set */
		TPM0->SC |= 0x80; /* clear TOF */
	}
}

// NOTA: TPM0 está configurado para contar en milisegundos, no microsegundos
/* Delay function */
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
