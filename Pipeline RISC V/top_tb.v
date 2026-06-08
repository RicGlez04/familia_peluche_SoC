`timescale 1ns/1ps

module top_tb();

// 1. Señales de estímulo (Entradas del procesador)
reg Clk;
reg rst;

// 2. Instancia del módulo Top del Procesador (DUT)
top DUT (
    .Clk(Clk),
    .rst(rst)
);

// 3. Generador de Reloj Continuo (Ciclo de 10 unidades de tiempo)
always begin
    Clk = 0; #5;
    Clk = 1; #5;
end

// 4. Secuencia de ejecución de la prueba
initial
begin
    $display("---- Iniciando Simulación del Procesador Pipeline ----");
    
    // Activamos reset en alto para limpiar todos los registros de segmentación
    rst = 1;
    #12; // Esperamos un ciclo y medio de reloj
    
    // Liberamos el reset para que comience el procesamiento
    rst = 0;
    
    // Dejamos correr el pipeline por 200ns para que procese el archivo .mem
    #200;
    
    $display("---- Simulación Finalizada con Éxito ----");
    $stop; // Detiene la simulación en ModelSim/Questa
end

endmodule