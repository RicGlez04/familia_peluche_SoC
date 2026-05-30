// Este modulo integra el main decoder y ALU decoder
module ControlUnit(
    input [6:0] op,
    input [2:0] funct3,
    input funct7,
    input zero,

    output PCSrc,
    output ResultSrc,
    output MemWrite,
    output [2:0] ALUControl,
    output ALUSrc,
    output [1:0] ImmSrc,
    output RegWrite
);

// Señal intermedias
wire [1:0] ALUOp;

// El Control Unit toma el ingreso de la instrucción completa y separa las señales de control
MainDecoder mainDecoder(
    // Inputs 
    .op(op),
    .zero(zero),

    // Outputs
    .PCSrc(PCSrc),
    .ResultSrc(ResultSrc),
    .MemWrite(MemWrite),
    .ALUSrc(ALUSrc),
    .ImmSrc(ImmSrc),
    .RegWrite(RegWrite),
    .ALUOp(ALUOp)
);

ALUDecode ALUDecoder(
    // Inputs
    .ALUOp(ALUOp),
    .funct3(funct3),
    .funct7(funct7),
    .op5(op[5]),
    
    // Outputs
    .ALUControl(ALUControl)
);


endmodule