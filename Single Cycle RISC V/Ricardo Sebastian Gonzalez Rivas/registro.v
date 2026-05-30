module registro(
    // Inputs
    input wire clk,
    input wire rst,
    input [31:0] PCin,

    // Outputs
    output [15:0] PCout
);

// Se espera un ciclo de reloj para arrojar PC
always @(posedge clk or posedge rst)
begin
    if(rst)
        PCout <= 32'b0;   // Reiniciar PC a cero
    else
        PCOut <= PCin;    // Guardar PC de input
end

endmodule