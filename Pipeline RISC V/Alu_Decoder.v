// Función: Decodificador de la ALU que define la operación específica a realizar según el tipo de instrucción.
module Alu_Decoder(
	input [1:0] AluOp,
	input [2:0] funct3,
	input funct7, Op,
	output reg [2:0] Instruction
);

always @(*)
	begin
		// Selecciona el tipo de operación: R/I (2), restas (1) o sumas (0).
		case(AluOp)
			0: Instruction=0;
			1: Instruction=1;
			2:
				// Identifica la operación tipo R o I.
				case(funct3)
					0:
						// Diferencia entre operaciones como suma y resta
						case({Op,funct7})
							3: Instruction=1;
							default: Instruction=0;
						endcase
					2: Instruction=5;
					6: Instruction=3;
					7: Instruction=2;
					default: Instruction=0;
				endcase
			default: Instruction=0;
		endcase

	end
endmodule