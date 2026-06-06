// Función: Realiza operaciones matemáticas y lógicas.
module Alu(
	input [31:0] An, Bn, 
	input [2:0] AluControl,
	output reg [31:0] Alu_result,
	output reg ZeroE
);

always @(*)
	begin
		// Selecciona la operación específica según el código de control recibido.
		case(AluControl)
			0: Alu_result=An + Bn;
			1: Alu_result=An - Bn;
			2: Alu_result=An & Bn;
			3: Alu_result=An | Bn;
			5: Alu_result=An << Bn[4:0];
			default: Alu_result=0;
		endcase
		
		if(Alu_result==0)
			ZeroE=1;
		else
			ZeroE=0;
	end
endmodule