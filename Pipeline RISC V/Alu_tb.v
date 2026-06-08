module Alu_tb();

reg [31:0] An, Bn;
reg [2:0] AluControl;
wire [31:0] Alu_result;
wire ZeroE;

Alu DUT (
.An(An),
.Bn(Bn),
.AluControl(AluControl),
.Alu_result(Alu_result),
.ZeroE(ZeroE)
);

initial
	begin
		// Suma 5 + 3 = 8
		An = 5;
		Bn = 3;
		AluControl = 0;
		#10;

		// Resta 10 - 4 = 6
		An = 10;
		Bn = 4;
		AluControl = 1;
		#10;

		// Resta que da Cero 7 - 7 = 0 para activar ZeroE
		An = 7;
		Bn = 7;
		AluControl = 1;
		#10;

		// AND
		An = 32'b1100;
		Bn = 32'b1010;
		AluControl = 2; 
		#10;

		// OR
		An = 32'b1100;
		Bn = 32'b1010;
		AluControl = 3; 
		#10;

		// Shift Left 1 << 2 = 4
		An = 1;
		Bn = 2;
		AluControl = 5;
		#10;
	end

endmodule