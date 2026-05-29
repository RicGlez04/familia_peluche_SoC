module main decoder(
input [6:0] op,
input zero,
output reg Regwrite,
output reg [1:0]ImmSrc,
output reg ALUSrc,
output reg MemWrite,
output reg [1:0]ResultSrc,
output reg Branch,
output reg [1:0]ALUOp,
output reg Jump
);

always@(*)

case(op)
	
	3:begin
	Regwrite = 1;
	ImmSrc = 2b'00;
	ALUSrc = 1;
	MemWrite = 0;
	ResultSrc = 2b'01;
	Branch = 0;
	ALUOp = 2b'00;
	Jump = 0;
	end
	35:begin
	Regwrite = 1;
	ImmSrc = 2b'00;
	ALUSrc = 1;
	MemWrite = 0;
	ResultSrc = 2b'01;
	Branch = 0;
	ALUOp = 2b'00;
	Jump = 0;
	
	end
	


	endcase

endmodule