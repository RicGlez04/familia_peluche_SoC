// Función: Transforma los inmediatos de las instrucciones a 32 bits.
module Imm_Extend(
	input[1:0] ImmSrc,
	input[31:0] Instr,
	output reg[31:0] ImmExt
);

always@(*)
	begin
		// Reorganiza y extiende los bits del inmediato según el formato de la instrucción (I, S, B, J).
		case(ImmSrc)
			0: ImmExt={{20{Instr[31]}}, Instr[31:20]};
			1: ImmExt={{20{Instr[31]}}, Instr[31:25], Instr[11:7]};
			2: ImmExt={{19{Instr[31]}}, Instr[31], Instr[7], Instr[30:25], Instr[11:8], 1'b0};
			3: ImmExt={{12{Instr[31]}}, Instr[19:12], Instr[20], Instr[30:21], 1'b0};
			default: ImmExt=0;
		endcase
	end
endmodule