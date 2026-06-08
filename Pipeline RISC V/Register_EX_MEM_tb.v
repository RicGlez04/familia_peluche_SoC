module Register_EX_MEM_tb();

reg clk, rst, MemWriteE, RegWriteE;
reg [1:0] ResultSrcE;
reg [31:0] Alu_result, WriteDataE, PCPlus4E;
reg [4:0] RdE;

wire MemWriteM, RegWriteM;
wire [1:0] ResultSrcM;
wire [31:0] Alu_resultM, WriteDataM, PCPlus4M;
wire [4:0] RdM;

Register_EX_MEM DUT (
.clk(clk),
.rst(rst),
.MemWriteE(MemWriteE),
.RegWriteE(RegWriteE),
.ResultSrcE(ResultSrcE),
.Alu_result(Alu_result),
.WriteDataE(WriteDataE),
.PCPlus4E(PCPlus4E),
.RdE(RdE),
.MemWriteM(MemWriteM),
.RegWriteM(RegWriteM),
.ResultSrcM(ResultSrcM),
.Alu_resultM(Alu_resultM),
.WriteDataM(WriteDataM),
.PCPlus4M(PCPlus4M),
.RdM(RdM)
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
		// Activar Reset
		rst = 1;
		MemWriteE = 1; RegWriteE = 1; ResultSrcE = 2'b11;
		Alu_result = 32'hFFFF_FFFF; WriteDataE = 32'hAAAA_BBBB; PCPlus4E = 32'h0000_0008;
		RdE = 5'd5;
		#10; // salidas 'M' deben seguir en 0

		// Quitar Reset
		rst = 0;
		#10; // los valores de arriba pasan a las salidas 'M'

		//Cambiar los datos en la etapa E y que no pasen de inmediato
		MemWriteE = 0;
		RegWriteE = 1;
		ResultSrcE = 2'b01;
		Alu_result = 32'h1234_5678;
		WriteDataE = 32'h0000_1111;
		PCPlus4E = 32'h0000_000C;
		RdE = 5'd10;

		#3;  // las salidas 'M' con los datos viejos

		#7;  // llega el posedge clk, salidas 'M' se actualizan con nuevos valores 
	end

endmodule