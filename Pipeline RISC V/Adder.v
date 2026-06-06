// Función: Sumador utilizado para calcular las direcciones de salto y el incremento del PC.
module Adder(
	input [31:0] PC,
	input [31:0] variable,
	output reg [31:0] AdderOut
);

always@(*)
	AdderOut=PC+variable;

endmodule