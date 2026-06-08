module Register_MEM_WB_tb();

reg clk, rst, RegWriteM;
reg [1:0] ResultSrcM;
reg [31:0] Alu_resultM, ReadData, PCPlus4M;
reg [4:0] RdM;
wire RegWriteW;
wire [1:0] ResultSrcW;
wire [31:0] Alu_resultW, ReadDataW, PCPlus4W;
wire [4:0] RdW;

Register_MEM_WB DUT (
.clk(clk),
.rst(rst),
.RegWriteM(RegWriteM),
.ResultSrcM(ResultSrcM),
.Alu_resultM(Alu_resultM),
.ReadData(ReadData),
.PCPlus4M(PCPlus4M),
.RdM(RdM),
.RegWriteW(RegWriteW),
.ResultSrcW(ResultSrcW),
.Alu_resultW(Alu_resultW),
.ReadDataW(ReadDataW),
.PCPlus4W(PCPlus4W),
.RdW(RdW)
);

always
	begin
		 clk = 0;
		 #5;
		 clk = 1;
		 #5;
	end

initial
	begin
		// Reset inicial
		rst = 1;
		RegWriteM = 1; ResultSrcM = 2'b10;
		Alu_resultM = 32'h5555_5555; ReadData = 32'h3333_3333; PCPlus4M = 32'h0000_0010;
		RdM = 5'd15;
		#10;
		rst = 0;
		#5;

		// datos pasan a la etapa W
		#5;

		// modificar los datos en la etapa M y checar que esperen al clk
		RegWriteM = 0;
		ResultSrcM = 2'b01;
		Alu_resultM = 32'h9999_9999;
		ReadData = 32'h7777_7777;
		PCPlus4M = 32'h0000_0014;
		RdM = 5'd20;

		#3; // salidas 'W' deben seguir congeladas con datos del viejos

		#7; // salidas se actualizan con los valores nuevos
	end
	
endmodule