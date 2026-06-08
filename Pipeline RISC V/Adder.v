// Función: Sumador utilizado para calcular las direcciones de salto y el incremento del PC
// dentro de la etapa Exectue.
module Adder(
	input [31:0] PC,
	input [31:0] variable,      // Inmediato 
	output reg [31:0] AdderOut  // Suma del inmediato y PC
);

always@(*)
	AdderOut=PC+variable;       // Asignación directa

endmodule
