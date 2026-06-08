// Función: Almacena y actualiza la dirección de memoria de la instrucción actual.
module ProgramCounter(
	input clk, rst, StallF,
	input [31:0] PCNext,
	output reg [31:0] PC
);

// Registro que se actualiza en clk, rst o StallF
always@(posedge clk or posedge rst)
	begin
		if(rst)
			PC<=0;
		else if(StallF)
			PC<=PC;
		else
			PC<=PCNext;
	end
endmodule
