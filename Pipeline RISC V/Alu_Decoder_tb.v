module Alu_Decoder_tb();

reg [1:0] AluOp;
reg [2:0] funct3;
reg funct7, Op;
wire [2:0] Instruction;

Alu_Decoder DUT (
.AluOp(AluOp),
.funct3(funct3),
.funct7(funct7),
.Op(Op),
.Instruction(Instruction)
);

initial
	begin
	// AluOp = 0 (Tipico para Load/Store, debe dar Suma = 0)
	AluOp = 0;
	funct3 = 0; funct7 = 0; Op = 0;
	#10;

	//AluOp = 1 (Tipico para Branch, debe dar Resta = 1)
	AluOp = 1;
	#10;

	// AluOp = 2 (Tipo R/I) y funct3 = 0 (Suma/Resta)
		// Op=1 y funct7=1 ({Op,funct7} = 3) debe dar Resta = 1
	AluOp = 2;
	funct3 = 0;
	Op = 1;
	funct7 = 1;
	#10;

		// Op=0 y funct7=0 ({Op,funct7} = 0) debe dar Suma = 0
	Op = 0;
	funct7 = 0;
	#10;

	// AluOp = 2 y funct3 = 7, debe dar AND = 2
	funct3 = 7;
	#10;

	// AluOp = 2 y funct3 = 6, debe dar OR = 3
	funct3 = 6;
	#10;

	// AluOp = 2 y funct3 = 2, debe dar Shift Left
	funct3 = 2;
	#10;
	end

endmodule