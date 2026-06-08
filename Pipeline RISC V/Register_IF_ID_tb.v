module Register_IF_ID_tb();

reg clk, rst, StallD, FlushD;
reg [31:0] RD, PC, PCPlus4F;
wire [31:0] InstrD, PCD, PCPlus4D;

Register_IF_ID DUT (
.clk(clk),
.rst(rst),
.StallD(StallD),
.FlushD(FlushD),
.RD(RD),
.PC(PC),
.PCPlus4F(PCPlus4F),
.InstrD(InstrD),
.PCD(PCD),
.PCPlus4D(PCPlus4D)
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
		rst = 1; StallD = 0; FlushD = 0;
		RD = 32'h00510113; PC = 32'h0000_0000; PCPlus4F = 32'h0000_0004;
		#10;
		rst = 0;
		#5;

		// Primera instruccion
		#5;

		// Segunda instruccion en la entrada
		RD = 32'h00512223; PC = 32'h0000_0004; PCPlus4F = 32'h0000_0008;
		#10; // salidas se actualizan

		// Prueba de STALL Decode
		// Activamos StallD y cambiamos la entrada a una tercera instruccion.
		StallD = 1;
		RD = 32'hFE000CE3; PC = 32'h0000_0008; PCPlus4F = 32'h0000_000C;
		#10; // salidas congeladas en la segunda instrucción

		// Prueba de FLUSH
		// desactivamos StallD y activamos FlushD¿
		StallD = 0;
		FlushD = 1;
		#10; // todo a 0

		// desactivar FlushD
		FlushD = 0;
		#10;
	end

endmodule