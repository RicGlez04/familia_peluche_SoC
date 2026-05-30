module Extender(
    // Inputs
    input [1:0] ImmSrc,      // Señal de control dependiendo de la instrucción
    input [31:0] Instruction // Recibe señal completa

    // Outputs
    output reg [31:0] ImmExt
);

always @(*)
begin
    case(ImmSrc)
        2'b00: ImmExt = {{20{Instruction[31]}}, Instruction[31:20]};                     // Instruccion I-type
        2'b01: ImmExt = {{20{Instruction[31]}}, Instruction[31:25], Instruction[11:7]};  // Instruccion S-type
        2'b10: ImmExt = {{19{Instruction[31]}}, Instruction[31], Instruction[7], 
                          Instruction[30:25], Instruction[11:8], 1'b0};                  // Instruccion B-type
        default: ImmExt = 32'b0;
    endcase
end

endmodule