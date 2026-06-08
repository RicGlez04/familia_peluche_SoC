// Función: Lógica de control de saltos que determina si el PC debe tomar una nueva dirección.
module BranchComparator(
	input Branch, Zero, Jump,     // Señales binarias
	output reg PCSrc
);

always@(*)
	PCSrc=(Branch & Zero) | Jump; // Señal de control en caso de Branch o Jump

endmodule
