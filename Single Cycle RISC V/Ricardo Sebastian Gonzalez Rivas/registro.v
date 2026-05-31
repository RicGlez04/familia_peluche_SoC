module registro(
    // Entradas
    input wire clk,
    input wire rst,
    input [31:0] PCin,

    // Salidas
    output reg [31:0] PCout
);

// Se espera un ciclo de reloj para arrojar PC
always @(posedge clk or posedge rst)
begin
    if(rst)
        PCout <= 32'b0;   // Reiniciar PC a cero
    else
        PCout <= PCin;    // Guardar PC de input
end

endmodule
