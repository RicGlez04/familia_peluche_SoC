// Función: Realiza operaciones matemáticas y lógica. El ALU principal
// dentro de la etapa Execute.
module Alu(
	input [31:0] An, Bn,           // SrcA, SrcB
	input [2:0] AluControl,        // Código de control
	output reg [31:0] Alu_result,  // Resultado de la operación
	output reg ZeroE			   // Señal binaria para Branch
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

		// Operación Zero en caso de Branch
		if(Alu_result==0)
			ZeroE=1;
		else
			ZeroE=0;
	end
endmodule
