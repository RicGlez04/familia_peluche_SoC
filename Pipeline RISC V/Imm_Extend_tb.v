module Imm_Extend_tb();

reg [1:0] ImmSrc;
reg [31:0] Instr;
wire [31:0] ImmExt;

Imm_Extend DUT (
.ImmSrc(ImmSrc),
.Instr(Instr),
.ImmExt(ImmExt)
);

initial
	begin
		// Formato Tipo I (ImmSrc = 0)
		// inmediato positivo (Inmediato esperado: 12'h005 -> 32'h0000_0005)
		ImmSrc = 0;
		Instr = 32'h00510113; // addi x2, x2, 5
		#10;

		// inmediato negativo (Inmediato esperado: 12'hFFF -> 32'hFFFF_FFFF por extension de signo)
		Instr = 32'hFFF10113; // addi x2, x2, -1
		#10;

		// Formato Tipo S (ImmSrc = 1)
		// mapea bits [31:25] y [11:7]
		// bits de la instruccion [31:25]=7'b0000000, [11:7]=5'b00100
		ImmSrc = 1;
		Instr = 32'h00512223; 
		#10;

		// Formato Tipo B (ImmSrc = 2)
		// multiplica por 2 (bit 0 en 0)
		// forza el bit de signo [31] en 1
		ImmSrc = 2;
		Instr = 32'hFE000CE3; 
		#10;

		// Formato Tipo J (ImmSrc = 3)
		// JAL, reorganiza 20 bits y añade un 0 al final
		ImmSrc = 3;
		Instr = 32'h004000ef; 
		#10;
	end
endmodule