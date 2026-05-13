#include <MKL25Z4.H>

int main(void) {
    // 1. Habilitar relojes para Puerto B y módulo TPM2
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
    SIM->SCGC6 |= SIM_SCGC6_TPM2_MASK;

    // 2. Configurar PTB18 (LED rojo) como salida de TPM2_CH0 (Alt 3)
    PORTB->PCR[18] = PORT_PCR_MUX(3);

    // 3. Seleccionar MCGFLLCLK (41.94 MHz) como fuente de reloj para el timer
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);

    // 4. Configuración del Timer TPM2
    TPM2->SC = 0; // Apagar para configurar

    // Modo: High-True Pulse (MSB=1, ELSB=1, ELSA=0)
    TPM2->CONTROLS[0].CnSC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK;

    // 6. Valores para 60 kHz
    // MOD = (41.94MHz / (Prescaler 16 * 60,000Hz)) - 1 = 43
    TPM2->MOD = 43;

    // Duty Cycle: 0 = Encendido total, 43 = Apagado total.
    // Para un ~33% de intensidad (considerando la inversión física):
    //25%
    //TPM2->CONTROLS[0].CnV =11;
    //50%
    //TPM2->CONTROLS[0].CnV =22;
    //75%
    //TPM2->CONTROLS[0].CnV =33;
    //100%
    TPM2->CONTROLS[0].CnV =44;

    // 7. Habilitar Interrupción por Desbordamiento (TOIE)
    // CMOD=1 (Inicia contador), PS=4 (Prescaler 1:16)
    TPM2->SC = TPM_SC_TOIE_MASK | TPM_SC_CMOD(1) | TPM_SC_PS(4);

    // 8. Habilitar el vector de interrupción en el NVIC
    NVIC_EnableIRQ(TPM2_IRQn);

    while (1) {
        // El hardware genera el PWM automáticamente
    }
}

// Handler de la interrupción del TPM2
void TPM2_IRQHandler(void) {
    // Verificar si la bandera de desbordamiento está activa
    if (TPM2->SC & TPM_SC_TOF_MASK) {

        // --- Tu lógica aquí ---

        // IMPORTANTE: Limpiar la bandera escribiendo un 1
        TPM2->SC |= TPM_SC_TOF_MASK;
    }
}
