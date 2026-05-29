module TEC_bench_tb();

// Entradas al top se declaran como 'reg' para poder controlarlas
    reg clk;
    reg rst;

    // Salidas del top se declaran como 'wire' para observarlas
    // (Nota: puse pc_tb como wire porque en tu top.v está como input, 
    // pero internamente lo estás sobreescribiendo. Lee la nota abajo).
    wire [31:0] pc_tb;
    wire out_tb;

    // 1. Instanciación de tu procesador (Device Under Test)
    top dut (
        .Clk(clk),
        .rst(rst),
        .pc(pc_tb),
        .out(out_tb)
    );

    // 2. Generación de la señal de reloj (Cambia de estado cada 5 ns)
    always #5 clk = ~clk;

    // 3. Secuencia de prueba (El comportamiento)
    initial begin
        $display("Iniciando simulacion del procesador RISC-V...");

        // Estado inicial: reloj apagado y aplicamos el Reset
        clk = 0;
        rst = 1;

        // Mantenemos el reset por 10 ns para que todo se limpie
        #10;
        
        // Soltamos el reset para que el procesador empiece a ejecutar
        rst = 0;

        // Dejamos que el procesador corra por varios ciclos.
        // Tienes 3 instrucciones en tu instrMem.hex, le damos tiempo de sobra.
        #100;

        // Terminamos la simulación
        $display("Simulacion terminada.");
        $stop; 
    end

endmodule