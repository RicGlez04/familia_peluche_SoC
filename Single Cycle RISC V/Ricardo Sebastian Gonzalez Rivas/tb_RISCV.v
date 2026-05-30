`timescale 1ns / 1ps

module tb_RISCV();

    reg clk;
    reg rst;

    // Instancia del módulo Top
    riscv_single_cycle uut (
        .clk(clk),
        .rst(rst)
    );

    // Generador de Reloj (Periodo de 10ns -> 100MHz)
    always begin
        #5 clk = ~clk;
    end

    initial begin
        // Inicialización de señales
        clk = 0;
        rst = 1;
        
        // Esperamos 12ns y soltamos el reset
        #12;
        rst = 0;
        
        // Dejar correr la simulación por 100ns para ejecutar las instrucciones del .hex
        #100;
        
        $display("Simulación terminada de manera exitosa.");
        $finish;
    end

    // Monitoreo opcional para ver cambios en la consola del simulador
    initial begin
        $monitor("Tiempo=%0d ns | Reset=%b | PC=%h", $time, rst, uut.PC_wire);
    end

endmodule