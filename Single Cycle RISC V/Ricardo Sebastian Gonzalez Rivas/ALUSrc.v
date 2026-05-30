module ALUSrc(
    // Inputs
    input ALUSrc,        // Señal de control
    input [31:0] RD2,    // Dato de registro
    input [31:0] ImmExt, // Dato inmediato

    // Outputs 
    output reg [31:0] SrcB
);

always @(*)
begin
    case(ALUSrc)
        1'b0: SrcB = RD2;
        1'b1: SrcB = ImmExt;
    endcase
end

endmodule