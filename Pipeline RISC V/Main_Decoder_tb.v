module Main_Decoder_tb();

reg [6:0] op;

wire RegWrite, ALUSrc, MemWrite, Branch, Jump;
wire [1:0] ImmSrc, ResultSrc, AluOp;

Main_Decoder DUT (
.op(op),
.RegWrite(RegWrite),
.ALUSrc(ALUSrc),
.MemWrite(MemWrite),
.Branch(Branch),
.Jump(Jump),
.ImmSrc(ImmSrc),
.ResultSrc(ResultSrc),
.AluOp(AluOp)
);

initial
	begin
		// op = 3 (Instrucciones Load)
		op = 3;
		#10;

		// op = 35 (Instrucciones Store)
		op = 35;
		#10;

		// op = 51 (Instrucciones Tipo R)
		op = 51;
		#10;

		// op = 99 (Instrucciones Branch)
		op = 99;
		#10;

		// op = 19 (Instrucciones Tipo I)
		op = 19;
		#10;

		// op = 111 (Instruccion de salto)
		op = 111;
		#10;

		// default (Op no valido o desconocido)
		op = 0;
		#10;
	end

endmodule