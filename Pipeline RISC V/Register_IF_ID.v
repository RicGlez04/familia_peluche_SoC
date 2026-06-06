module Register_IF_ID(
	input clk, rst, StallD, FlushD,
	input [31:0] RD, PC, PCPlus4F,
	output reg [31:0] InstrD, PCD, PCPlus4D
);

always@(posedge clk or posedge rst)
	begin
		if(rst)
			begin
				InstrD<=0; 
				PCD<=0;
				PCPlus4D<=0;
			end
		else if(FlushD)
			begin
				InstrD<=0; 
				PCD<=0;
				PCPlus4D<=0;
			end
		else if(StallD)
			begin
				InstrD<=InstrD; 
				PCD<=PCD;
				PCPlus4D<=PCPlus4D;
			end
		else
			begin
				InstrD<=RD; 
				PCD<=PC;
				PCPlus4D<=PCPlus4F;
			end
			
	end
endmodule	