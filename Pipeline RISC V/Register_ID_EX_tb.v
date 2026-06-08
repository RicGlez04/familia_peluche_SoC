module Register_ID_EX_tb();

reg clk, rst, FlushE, MemWriteD, AluSrcD, RegWriteD, JumpD, BranchD;
reg [1:0] ResultSrcD;
reg [2:0] AluControlD;
reg [31:0] PCD, ExtImmD, PCPlus4D, RD1, RD2;
reg [4:0] Rs1D, Rs2D, RdD;
wire MemWriteE, AluSrcE, RegWriteE, JumpE, BranchE;
wire [1:0] ResultSrcE;
wire [2:0] AluControlE;
wire [31:0] PCE, ExtImmE, PCPlus4E, RD1E, RD2E;
wire [4:0] Rs1E, Rs2E, RdE;

Register_ID_EX DUT (
.clk(clk), .rst(rst), .FlushE(FlushE), .MemWriteD(MemWriteD), .AluSrcD(AluSrcD), 
.RegWriteD(RegWriteD), .JumpD(JumpD), .BranchD(BranchD), .ResultSrcD(ResultSrcD), 
.AluControlD(AluControlD), .PCD(PCD), .ExtImmD(ExtImmD), .PCPlus4D(PCPlus4D), 
.RD1(RD1), .RD2(RD2), .Rs1D(Rs1D), .Rs2D(Rs2D), .RdD(RdD),
.MemWriteE(MemWriteE), .AluSrcE(AluSrcE), .RegWriteE(RegWriteE), .JumpE(JumpE), 
.BranchE(BranchE), .ResultSrcE(ResultSrcE), .AluControlE(AluControlE), .PCE(PCE), 
.ExtImmE(ExtImmE), .PCPlus4E(PCPlus4E), .RD1E(RD1E), .RD2E(RD2E), 
.Rs1E(Rs1E), .Rs2E(Rs2E), .RdE(RdE)
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
		// reset inicial
		rst = 1; FlushE = 0;
		MemWriteD = 1; RegWriteD = 1; AluControlD = 3'b010;
		PCD = 32'h0000_0004; RD1 = 32'h1111_1111; RD2 = 32'h2222_2222;
		Rs1D = 5'd1; Rs2D = 5'd2; RdD = 5'd3;
		#10;
		rst = 0;
		#5;

		// datos pasan al Execute en el posedge
		#5; // posedge cargue los datos de arriba

		// cargar nuevos datos en la etapa Decode
		MemWriteD = 0; RegWriteD = 1; AluControlD = 3'b111;
		PCD = 32'h0000_0008; RD1 = 32'hAAAA_BBBB; RD2 = 32'hCCCC_DDDD;
		Rs1D = 5'd10; Rs2D = 5'd11; RdD = 5'd12;
		#10; // salidas se actualizan

		// limpieza de la etapa Execute
		// Activamos FlushE, todas las salidas deben caer a 0
		FlushE = 1;
		#10;

		// desactivar FlushE
		FlushE = 0;
		#10;
	end

endmodule