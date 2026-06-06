// Función: Unifica los decodificadores y la lógica de saltos del procesador.
module ControlUnit(
	input [6:0] op,
	input [2:0] funct3,
	input funct7,
	output [2:0] AluControlD,
	output MemWriteD, AluSrcD, RegWriteD, JumpD, BranchD,
	output [1:0] ImmSrcD, ResultSrcD
);

wire [1:0] AluOp;

Main_Decoder MainDecoder(
	.op(op),
	.RegWrite(RegWriteD), 
	.ALUSrc(AluSrcD), 
	.MemWrite(MemWriteD), 
	.Branch(BranchD), 
	.Jump(JumpD),
	.ImmSrc(ImmSrcD),
	.ResultSrc(ResultSrcD), 
	.AluOp(AluOp)
);

Alu_Decoder AluDecoder(
	.AluOp(AluOp),
	.funct3(funct3),
	.funct7(funct7), 
	.Op(op[5]),
	.Instruction(AluControlD)
);

endmodule
	