module ProgramCounter_tb();

reg clk, rst, StallF;
reg [31:0] PCNext;
wire [31:0] PC;

ProgramCounter DUT (
.clk(clk),
.rst(rst),
.StallF(StallF),
.PCNext(PCNext),
.PC(PC)
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
		// iniciar y reset activo
		rst = 1;
		StallF = 0;
		PCNext = 32'h0000_0004;
		#10;

		// quitar reset
		rst = 0;
		#10; // PCNext, cambia a 0x0000_0004

		// avanzar al siguiente PC 0x0000_0008
		PCNext = 32'h0000_0008;
		#10; // PC cambia

		// activar StallF
		StallF = 1;
		PCNext = 32'h0000_000C; // cambiamos PCNext a 12, pero esta en stall
		#10; // PC se queda en 0x0000_0008

		// desactivar StallF
		StallF = 0;
		#10; // PCNext (0x0000_000C)
	end

endmodule
