module Register_EX_MEM(
	input clk, rst, MemWriteE, RegWriteE,
	input [1:0] ResultSrcE,
	input [31:0] Alu_result, WriteDataE, PCPlus4E,
	input [4:0] RdE,
	output reg MemWriteM, RegWriteM,
	output reg [1:0] ResultSrcM,
	output reg [31:0] Alu_resultM, WriteDataM, PCPlus4M,
	output reg [4:0] RdM
);

always@(posedge clk or posedge rst)
	begin
		if(rst)
			begin
				MemWriteM<=0;
				RegWriteM<=0;
				ResultSrcM<=0;
				Alu_resultM<=0;
				WriteDataM<=0;
				PCPlus4M<=0;
				RdM<=0;
			end
		else
			begin
				MemWriteM<=MemWriteE;
				RegWriteM<=RegWriteE;
				ResultSrcM<=ResultSrcE;
				Alu_resultM<=Alu_result;
				WriteDataM<=WriteDataE;
				PCPlus4M<=PCPlus4E;
				RdM<=RdE;
			end
			
	end
endmodule	