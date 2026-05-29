module main_decoder(
input [6:0] op,
input zero,
output reg Regwrite,
output reg [1:0]ImmSrc,
output reg ALUSrc,
output reg MemWrite,
output reg [1:0]ResultSrc,
output reg PcSrc,
output reg [1:0]ALUOp
);

reg Branch;

always@(*)

case(op)
	//lw
	3:begin
	Regwrite = 1;
	ImmSrc = 2'b00;
	ALUSrc = 1;
	MemWrite = 0;
	ResultSrc = 2'b01;
	Branch = 0;
	ALUOp = 2'b00;
	
	end
	//sw
	35:begin
	Regwrite = 1;
	ImmSrc = 2'b00;
	ALUSrc = 1;
	MemWrite = 0;
	ResultSrc = 2'b01;
	Branch = 0;
	ALUOp = 2'b00;
	
	end
	//Rtype
	51:begin
	Regwrite = 1;
	ImmSrc = 2'b00;
	ALUSrc = 0;
	MemWrite = 0;
	ResultSrc = 2'b00;
	Branch = 0;
	ALUOp = 2'b10;	
	
	end
	//Beq
	99:begin
	Regwrite = 0;
	ImmSrc = 2'b10;
	ALUSrc = 0;
	MemWrite = 0;
	ResultSrc = 2'b00;
	Branch = 1;
	ALUOp = 2'b01;	

	end
		
	//adi
	19:begin
	Regwrite = 1;
	ImmSrc = 2'b00;
	ALUSrc = 1;
	MemWrite = 0;
	ResultSrc = 2'b00;
	Branch = 0;
	ALUOp = 2'b10;	
	
	end
	
	//jal // no implementa jump and link
	111:begin
	Regwrite = 1;
	ImmSrc = 2'b11;
	ALUSrc = 0;
	MemWrite = 0;
	ResultSrc = 2'b10;
	Branch = 0;
	ALUOp = 2'b00;	
	
	end
	
	endcase

	
	always@(*)
	begin
	PcSrc = Branch & zero;
	end
	
endmodule