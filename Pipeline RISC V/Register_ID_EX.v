module Register_ID_EX(
	input clk, rst, FlushE, MemWriteD, AluSrcD, RegWriteD, JumpD, BranchD,
	input [1:0] ResultSrcD,
	input [2:0] AluControlD,
	input [31:0] PCD, ExtImmD, PCPlus4D, RD1, RD2,
	input [4:0] Rs1D, Rs2D, RdD,
	output reg MemWriteE, AluSrcE, RegWriteE, JumpE, BranchE,
	output reg [1:0] ResultSrcE,
	output reg [2:0] AluControlE,
	output reg [31:0] PCE, ExtImmE, PCPlus4E, RD1E, RD2E,
	output reg [4:0] Rs1E, Rs2E, RdE
);

always@(posedge clk or posedge rst)
	begin
		if(rst)
			begin
				MemWriteE<=0;
				AluSrcE<=0;
				RegWriteE<=0;
				JumpE<=0;
				BranchE<=0;
				ResultSrcE<=0;
				AluControlE<=0;
				PCE<=0; 
				Rs1E<=0; 
				Rs2E<=0;
				RdE<=0; 
				ExtImmE<=0; 
				PCPlus4E<=0;
				RD1E<=0;
				RD2E<=0;
			end
		else if(FlushE)
			begin
				MemWriteE<=0;
				AluSrcE<=0;
				RegWriteE<=0;
				JumpE<=0;
				BranchE<=0;
				ResultSrcE<=0;
				AluControlE<=0;
				PCE<=0; 
				Rs1E<=0; 
				Rs2E<=0;
				RdE<=0; 
				ExtImmE<=0; 
				PCPlus4E<=0;
				RD1E<=0;
				RD2E<=0;
			end
		else
			begin
				MemWriteE<=MemWriteD;
				AluSrcE<=AluSrcD;
				RegWriteE<=RegWriteD;
				JumpE<=JumpD;
				BranchE<=BranchD;
				ResultSrcE<=ResultSrcD;
				AluControlE<=AluControlD;
				PCE<=PCD; 
				Rs1E<=Rs1D; 
				Rs2E<=Rs2D;
				RdE<=RdD; 
				ExtImmE<=ExtImmD; 
				PCPlus4E<=PCPlus4D;
				RD1E<=RD1;
				RD2E<=RD2;
			end
			
	end
endmodule