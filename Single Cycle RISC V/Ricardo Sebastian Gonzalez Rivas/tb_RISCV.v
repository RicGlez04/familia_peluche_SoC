`timescale 1ns / 1ps

module tb_RISCV();

    reg clk;
    reg rst;

    // Instancia del módulo Top
    RISCV_Single_Cycle DUT (
        .clk(clk),
        .rst(rst)
    );

    // Generar el reloj
    always begin
        #5 clk = ~clk;
    end

    initial begin
        // Inicialización de señales
        clk = 0;
        rst = 1;
        
        #15;      // Esperar 12 ciclos
        rst = 0;  // Soltar señal de reset
        
        // Dejar correr la simulación para ejecutar TODAS las instrucciones
        #200;
        
        $display("Simulación terminada de manera exitosa.");
        $finish;
    end

    // Monitoreo opcional para ver cambios en la consola del simulador
    initial begin
        $monitor("Tiempo=%0d ns | Reset=%b | PC=%h", $time, rst, DUT.PC_wire);
    end

    initial begin
        $dumpfile("simulacion.vcd");
        $dumpvars(0, tb_RISCV);
    end

endmodule
