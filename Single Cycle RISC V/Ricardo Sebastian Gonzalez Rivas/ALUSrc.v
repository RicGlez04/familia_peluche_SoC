// Este módulo es el multiplexor que elige entre registro
// o inmediato como entrada la ALU principal
module ALUSrc(
    // Entradas
    input ALUSrc,        // Señal de control
    input [31:0] RD2,    // Dato de registro
    input [31:0] ImmExt, // Dato inmediato

    // Salidas 
    output reg [31:0] SrcB
);

// Dependiendo de la señal de control ALUSrc, se elige entre inmediato o registro
always @(*)
begin
    case(ALUSrc)
        1'b0: SrcB = RD2;
        1'b1: SrcB = ImmExt;
    endcase
end

endmodule
