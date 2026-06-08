// Función: Registro entre etapas Memory y Writeback.
// Las señales pasan de M a W.
module Register_MEM_WB(
	input clk, rst, RegWriteM,
	input [1:0] ResultSrcM,
	input [31:0] Alu_resultM, ReadData, PCPlus4M,
	input [4:0] RdM,
	
	
	output reg RegWriteW,
	output reg [1:0] ResultSrcW,
	output reg [31:0] Alu_resultW, ReadDataW, PCPlus4W,
	output reg [4:0] RdW
);

// Control de datos con clk o rst
always@(posedge clk or posedge rst)
	begin
		// Reinicio de datos con rst
		if(rst)
			begin
				RegWriteW<=0;
				ResultSrcW<=0;
				Alu_resultW<=0;
				ReadDataW<=0;
				PCPlus4W<=0;
				RdW<=0;
			end
		// Paso de datos con clk
		else
			begin
				RegWriteW<=RegWriteM;
				ResultSrcW<=ResultSrcM;
				Alu_resultW<=Alu_resultM;
				ReadDataW<=ReadData;
				PCPlus4W<=PCPlus4M;
				RdW<=RdM;
			end
			
	end
endmodule	
