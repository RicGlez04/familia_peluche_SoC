module ControlUnit_tb();

reg [6:0] op;
reg [2:0] funct3;
reg funct7;

wire [2:0] AluControlD;
wire MemWriteD, AluSrcD, RegWriteD, JumpD, BranchD;
wire [1:0] ImmSrcD, ResultSrcD;

ControlUnit DUT (
.op(op),
.funct3(funct3),
.funct7(funct7),
.AluControlD(AluControlD),
.MemWriteD(MemWriteD),
.AluSrcD(AluSrcD),
.RegWriteD(RegWriteD),
.JumpD(JumpD),
.BranchD(BranchD),
.ImmSrcD(ImmSrcD),
.ResultSrcD(ResultSrcD)
);

initial
	begin
	// Tipo-R (op = 7'b0110011)
	// Ejemplo: ADD
	op = 7'b0110011;
	funct3 = 3'b000;
	funct7 = 1'b0;
	#10;

	// Tipo-R (op = 7'b0110011)
	// Ejemplo: SUB (Cambia funct7)
	funct7 = 1'b1;
	#10;

	// Instrucción Load (op = 7'b0000011)
	// Ejemplo: LW
	op = 7'b0000011;
	funct3 = 3'b010; // lw
	funct7 = 1'b0;
	#10;

	// Instrucción Store (op = 7'b0100011)
	// Ejemplo: SW
	op = 7'b0100011;
	funct3 = 3'b010; // sw
	#10;

	// Instrucción de Salto Condicional (op = 7'b1100011)
	// Ejemplo: BEQ
	op = 7'b1100011;
	funct3 = 3'b000; // beq
	#10;

	// Instrucción de Salto Incondicional (op = 7'b1101111)
	// Ejemplo: JAL
	op = 7'b1101111;
	#10;
	end
endmodule