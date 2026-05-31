module PCNext(
    // Señal de control para PC 
    input PCSrc,

    // PCs de entrada
    input [31:0] PCPlus4,
    input [31:0] PCTarget,

    // PC de salida
    output reg [31:0] PCNext
);

always @(*)
begin
    case(PCSrc)
        1'b0: PCNext = PCPlus4;
        1'b1: PCNext = PCTarget;
        default: PCNext = PCNext;
    endcase
end

endmodule
