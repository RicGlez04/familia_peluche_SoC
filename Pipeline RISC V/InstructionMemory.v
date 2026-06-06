// Función: Memoria ROM que almacena el código máquina de las instrucciones del programa.
module InstructionMemory(
	input [31:0] A,
	output reg [31:0] RD
);

reg[31:0] rom [0:63];

initial begin
    $readmemh("C:/Diseno de Chips/Pipeline/programa.mem", rom);
end

always@(*)
	RD=rom[A[31:2]];

endmodule
	