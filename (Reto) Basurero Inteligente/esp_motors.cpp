//codigo ESP32
#include <Arduino.h>

// Pines Motor 1 (ORGANICO - Letra 'D') 
#define M1_A1 14 
#define M1_A2 26
#define M1_B1 27 
#define M1_B2 25 

// Pines Motor 2 (INORGANICO - Letra 'I') 
#define M2_A1 22 
#define M2_A2 21
#define M2_B1 23 
#define M2_B2 19 

// Secuencia Full-Step para maxima fuerza
const uint8_t secuencia[4] = { 0b1001, 0b1100, 0b0110, 0b0011 };

// ==========================================
// FUNCIONES MOTOR 1 (ORGANICO)
// ==========================================
void aplicarPasoM1(uint8_t patron) {
    digitalWrite(M1_A1, (patron & 0x08) >> 3);
    digitalWrite(M1_A2, (patron & 0x04) >> 2);
    digitalWrite(M1_B1, (patron & 0x02) >> 1);
    digitalWrite(M1_B2, (patron & 0x01));
}

void apagarM1() {
    digitalWrite(M1_A1, LOW); digitalWrite(M1_A2, LOW);
    digitalWrite(M1_B1, LOW); digitalWrite(M1_B2, LOW);
}

void abrirTapaOrganico() {
    // Abrir tapa (90 grados aprox = 100 pasos)
    for(int i=0; i<100; i++){
        aplicarPasoM1(secuencia[i%4]);
        delay(5);
    }
    apagarM1();
    
    delay(3000); // Dar 3 segundos para que el residuo caiga
    
    // Cerrar tapa (Girar en reversa)
    for(int i=0; i<100; i++){
        aplicarPasoM1(secuencia[3 - (i%4)]);
        delay(5);
    }
    apagarM1();
}

// ==========================================
// FUNCIONES MOTOR 2 (INORGANICO)
// ==========================================
void aplicarPasoM2(uint8_t patron) {
    digitalWrite(M2_A1, (patron & 0x08) >> 3);
    digitalWrite(M2_A2, (patron & 0x04) >> 2);
    digitalWrite(M2_B1, (patron & 0x02) >> 1);
    digitalWrite(M2_B2, (patron & 0x01));
}

void apagarM2() {
    digitalWrite(M2_A1, LOW); digitalWrite(M2_A2, LOW);
    digitalWrite(M2_B1, LOW); digitalWrite(M2_B2, LOW);
}

void abrirTapaInorganico() {
    // Abrir tapa (90 grados aprox)
    for(int i=0; i<100; i++){
        aplicarPasoM2(secuencia[i%4]);
        delay(5);
    }
    apagarM2();
    
    delay(3000); // Dar 3 segundos para que el residuo caiga
    
    // Cerrar tapa (Girar en reversa)
    for(int i=0; i<100; i++){
        aplicarPasoM2(secuencia[3 - (i%4)]);
        delay(5);
    }
    apagarM2();
}

void setup() {
    Serial.begin(115200);
    
    pinMode(M1_A1, OUTPUT); pinMode(M1_A2, OUTPUT);
    pinMode(M1_B1, OUTPUT); pinMode(M1_B2, OUTPUT);
    
    pinMode(M2_A1, OUTPUT); pinMode(M2_A2, OUTPUT);
    pinMode(M2_B1, OUTPUT); pinMode(M2_B2, OUTPUT);
    
    apagarM1();
    apagarM2();
}

void loop() {
    if (Serial.available() > 0) {
        char comando = Serial.read();
        
        if (comando == 'D') {
            Serial.println("Comando D -> Abriendo Organico (Motor 1)");
            abrirTapaOrganico();
        } 
        else if (comando == 'I') {
            Serial.println("Comando I -> Abriendo Inorganico (Motor 2)");
            abrirTapaInorganico();
        }
    }
}
