// Este módulo es el bloque completo, el Control Unit
// e integra el main decoder y ALU decoder
module ControlUnit(
    // Entradas
    input [6:0] op,     // Recibe el op completo
    input [2:0] funct3,
    input funct7,
    input zero,

    // Salidas
    output PCSrc,
    output [1:0] ResultSrc,
    output MemWrite,
    output [2:0] ALUControl,
    output ALUSrc,
    output [1:0] ImmSrc,
    output RegWrite
);

// Señal intermedias
wire [1:0] ALUOp;
wire Branch;

// El Control Unit toma el ingreso de la instrucción completa y separa las señales de control
mainDecoder MainDecoder(
    // Inputs 
    .op(op),

    // Outputs
    .ResultSrc(ResultSrc),
    .MemWrite(MemWrite),
    .ALUSrc(ALUSrc),
    .ImmSrc(ImmSrc),
    .RegWrite(RegWrite),
    .ALUOp(ALUOp),
    .Branch(Branch)
);

ALUDecoder ALUDecode(
    // Inputs
    .ALUOp(ALUOp),
    .funct3(funct3),
    .funct7(funct7),
    .op5(op[5]),       // Solo recibe el bit 5 del op
    
    // Outputs
    .ALUControl(ALUControl)
);

// Se calcula PCSrc
assign PCSrc = Branch & zero;

endmodule
