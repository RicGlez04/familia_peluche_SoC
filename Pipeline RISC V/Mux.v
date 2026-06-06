// Función: Multiplexor de 4 a 1.
module Mux(
	input [31:0] A, B, C, D,
	input [1:0] control,
	output reg [31:0] OutMux
);

always@(*)
	begin
		// Direcciona la entrada seleccionada hacia la salida del multiplexor.
		case(control)
			0: OutMux=A;
			1: OutMux=B;
			2: OutMux=C;
			3: OutMux=D;
			default: OutMux=A;
		endcase
	end

endmodule