module Register_File_tb();

reg [4:0] A1, A2, A3;
reg [31:0] WD3;
reg WE3, clk, rst;
wire [31:0] RD1, RD2;

Register_File DUT (
.A1(A1),
.A2(A2),
.A3(A3),
.WD3(WD3),
.WE3(WE3),
.clk(clk),
.rst(rst),
.RD1(RD1),
.RD2(RD2)
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
		rst = 1;
		WE3 = 0; A1 = 0; A2 = 0; A3 = 0; WD3 = 0;
		#10;
		rst = 0;
		#5;

		// escribir el dato 0x1234_5678 en el reg 5 (x5) (negedge clk)
		A3 = 5;
		WD3 = 32'h1234_5678;
		WE3 = 1;
		#10; // incluye el negedge

		// intentar escribir en el Registro 0 (solo lectura)
		A3 = 0;
		WD3 = 32'hFFFF_FFFF;
		WE3 = 1;
		#10;

		// lectura asíncrona de x5 y x0
		WE3 = 0;
		A1 = 5; // Apuntamos al registro 5, RD1 debe ser 0x1234_5678 de inmediato
		A2 = 0; // Apuntamos al registro 0, RD2 debe ser 0 de inmediato
		#10;
	end

endmodule