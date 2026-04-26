// Práctica 4: Interrupción de un contador

/* Al presionar la tecla '*' del teclado, se . */
/* Main program toggles red LED while waiting for an interrupt from switches. */

#include <MKL25Z4.H>

// Inicialización de timer TPM0
void TPM0_init(void);

// Funciones de delay
void delayMs(int n);

// Inicialización de la keypad
void keypad_init();

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


// Programa principal
int main(void) {


	TPM0_init(); // Inicializar registros temporizador

	__disable_irq(); /* disable all IRQs */

	unsigned char key;

	keypad_init();
	LCD_init();


	/* Configurar interrupción con tecla '*' */
	// ...

	__enable_irq(); /* global enable IRQs */

	while(1)
	{
		// ...
	}
}

/*  */
void Keypad_IRQHandler(void) {
	int i;
	// ...

	// ... /* clear interrupt flag */ }

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


// OJO: NO SE PUEDEN TENER DOS DELAYS de diferentes cantidades con el mismo TIMER
/* Delay function */
void delayMs(int n) {
	for(int i = 0; i < n; i++)
	{
		while((TPM0->SC & 0x80) == 0) { }
		/* wait until the TOF is set */
		TPM0->SC |= 0x80; /* clear TOF */
	}
}

/* Delay function */
void delayUs(int n) {
	while((TPM0->SC & 0x80) == 0)
	{ }
	/* wait until the TOF is set */
	TPM0->SC |= 0x80; /* clear TOF */
}
