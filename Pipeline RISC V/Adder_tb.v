module Adder_tb();

reg [31:0] PC, variable;
wire [31:0] AdderOut;

Adder DUT(
.PC(PC),
.variable(variable),
.AdderOut(AdderOut)
);

initial
	begin
		// Sumar 4 + 4
		PC = 4;
		variable = 4;
		#10;
		// Sumar 20 + 8
		PC = 20;
		variable = 8;
		#10;
	end

endmodule