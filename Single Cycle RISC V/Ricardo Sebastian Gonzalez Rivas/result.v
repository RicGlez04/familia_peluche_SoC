module result(
    // Inputs
    input [1:0] ResultSrc,
    input [31:0] ALUResult,  // Resultado directamente del ALU
    input [31:0] ReadData,   // Resultado tras leer memoria
    input [31:0] PCPlus4,    // Resultado tras aumentar PC por cuatro

    // Outputs
    output reg [31:0] Result     // Resultado seleccionado
);

always @(*)
begin
    case(ResultSrc)
        2'b00: Result = ALUResult;
        2'b01: Result = ReadData;
        2'b10: Result = PCPlus4;
        default: Result = 32'b0;
    endcase
end

endmodule