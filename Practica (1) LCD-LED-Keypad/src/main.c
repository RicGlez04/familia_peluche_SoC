#include <MKL25Z4.h>

// LCD control pins definitions
#define RS 0x04  // Register Select
#define RW 0x10  // Read/Write
#define EN 0x20  // Enable

// Delay function prototypes
void delayMs(int n);
void delayUs(int n);

// LCD function prototypes
void LCD_init(void);
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void LCD_print(char *text);
void LCD_sendNibble(unsigned char nibble);

// Keypad function prototypes
void keypad_init(void);
char keypad_getkey(void);

// RGB LED function prototypes
void RGB_init(void);
void RGB_off(void);
void RGB_setColor(char key);

int main(void){
    char key = 0;              // Variable to store pressed key
    char selectedColor = 0;   // Variable to store selected color

    // Configure TPM0 timer for delays
    SIM->SCGC6 |= 0x01000000;
    SIM->SOPT2 |= 0x01000000;
    TPM0->SC = 0;
    TPM0->SC = 0x02;
    TPM0->MOD = 0x2000;
    TPM0->SC |= 0x80;
    TPM0->SC |= 0x08;

    keypad_init(); // Initialize keypad
    LCD_init();    // Initialize LCD
    RGB_init();    // Initialize RGB LED

    // Display static text on LCD
    LCD_command(0x01); // Clear display
    delayMs(2);

    LCD_command(0x80); // First line
    LCD_print("Selecciona");

    LCD_command(0xC0); // Second line
    LCD_print("Color: ");

    while(1){
        key = keypad_getkey(); // Read pressed key

        if(key != 0){

            // Check if key is between '1' and '8'
            if(key >= '1' && key <= '8'){
                selectedColor = key;

                // Display selected number on LCD
                LCD_command(0xC7);
                LCD_data(selectedColor);
            }

            // Apply color when '*' is pressed
            if(key == '*' && selectedColor != 0){
                RGB_setColor(selectedColor);
            }

            delayMs(250); // Debounce delay
        }
    }
}

void keypad_init(void){
    SIM->SCGC5 |= 0x0800; // Enable clock for Port C

    // Configure Port C pins as GPIO
    PORTC->PCR[0] = 0x103;
    PORTC->PCR[1] = 0x103;
    PORTC->PCR[2] = 0x103;
    PORTC->PCR[3] = 0x103;
    PORTC->PCR[4] = 0x103;
    PORTC->PCR[5] = 0x103;
    PORTC->PCR[6] = 0x103;
    PORTC->PCR[7] = 0x103;

    PTC->PDDR = 0x0F; // Lower 4 bits as output (rows)
}

char keypad_getkey(void){
    // Keypad mapping
    const char keymap[4][4] = {
        {'1','2','3','A'},
        {'4','5','6','B'},
        {'7','8','9','C'},
        {'*','0','#','D'}
    };

    int row, col;

    // Scan each row
    for(row = 0; row < 4; row++){
        PTC->PDDR = 0x0F;   // Set rows as output
        PTC->PSOR = 0x0F;   // Set all rows high
        PTC->PCOR = (1 << row); // Pull current row low

        delayUs(2); // Small delay

        col = PTC->PDIR & 0xF0; // Read columns

        // Check if any key is pressed
        if(col != 0xF0){
            if((col & 0x10) == 0) return keymap[row][0];
            if((col & 0x20) == 0) return keymap[row][1];
            if((col & 0x40) == 0) return keymap[row][2];
            if((col & 0x80) == 0) return keymap[row][3];
        }
    }

    return 0; // No key pressed
}

void RGB_init(void){
    SIM->SCGC5 |= 0x0400; // Enable Port B clock
    SIM->SCGC5 |= 0x1000; // Enable Port D clock

    // Configure RGB pins as GPIO
    PORTB->PCR[18] = 0x100;
    PORTB->PCR[19] = 0x100;
    PORTD->PCR[1]  = 0x100;

    // Set pins as output
    PTB->PDDR |= (1 << 18) | (1 << 19);
    PTD->PDDR |= (1 << 1);

    RGB_off(); // Turn off LED initially
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

void LCD_init(void){
    SIM->SCGC5 |= 0x1000; // Enable Port D clock

    // Configure data pins (D4-D7)
    PORTD->PCR[4] = 0x100;
    PORTD->PCR[5] = 0x100;
    PORTD->PCR[6] = 0x100;
    PORTD->PCR[7] = 0x100;

    PTD->PDDR |= 0xF0; // Set as output

    SIM->SCGC5 |= 0x0200; // Enable Port A clock

    // Configure control pins (RS, RW, EN)
    PORTA->PCR[2] = 0x100;
    PORTA->PCR[4] = 0x100;
    PORTA->PCR[5] = 0x100;

    PTA->PDDR |= 0x34; // Set as output

    delayMs(20); // Wait for LCD to power up

    PTA->PCOR = RS | RW; // RS=0, RW=0

    // Initialization sequence for 4-bit mode
    LCD_command(0x30);
    delayMs(5);

    LCD_command(0x30);
    delayMs(1);

    LCD_command(0x30);
    delayMs(1);

    LCD_command(0x20);
    delayMs(1);

    // LCD configuration commands
    LCD_command(0x28); // 4-bit, 2 lines
    LCD_command(0x06); // Entry mode
    LCD_command(0x01); // Clear display
    LCD_command(0x0C); // Display ON
}

void LCD_sendNibble(unsigned char nibble){
    // Send upper 4 bits to LCD
    PTD->PDOR = (PTD->PDOR & 0x0F) | (nibble & 0xF0);

    // Enable pulse
    PTA->PSOR = EN;
    delayMs(1);
    PTA->PCOR = EN;
}

void LCD_command(unsigned char command){
    PTA->PCOR = RS | RW; // Command mode

    // Send high nibble
    LCD_sendNibble(command);

    // Send low nibble
    LCD_sendNibble(command << 4);

    delayMs(2); // Wait for command execution
}

void LCD_data(unsigned char data){
    PTA->PSOR = RS;  // Data mode
    PTA->PCOR = RW;  // Write mode

    // Send high nibble
    PTD->PDOR = (PTD->PDOR & 0x0F) | (data & 0xF0);
    PTA->PSOR = EN;
    delayMs(1);
    PTA->PCOR = EN;

    // Send low nibble
    PTD->PDOR = (PTD->PDOR & 0x0F) | ((data << 4) & 0xF0);
    PTA->PSOR = EN;
    delayMs(1);
    PTA->PCOR = EN;

    delayMs(2); // Wait for data write
}

void LCD_print(char *text){
    // Print string character by character
    while(*text){
        LCD_data(*text++);
    }
}

void delayUs(int n){
    int i;

    // Microsecond delay using TPM0
    for(i = 0; i < n; i++){
        while((TPM0->SC & 0x80) == 0){}
        TPM0->SC |= 0x80;
    }
}

void delayMs(int n){
    volatile int i;

    // Millisecond delay (approximate)
    while(n--){
        for(i = 0; i < 3500; i++){}
    }
}
