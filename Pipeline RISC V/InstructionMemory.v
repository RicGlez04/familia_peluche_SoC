// Función: Memoria ROM que almacena el código máquina de las instrucciones del programa.
module InstructionMemory(
	input [31:0] A,         // Dirección de la instrucción en memoria
	output reg [31:0] RD    // Registro destino
);

reg[31:0] rom [0:63];

//initial begin
//	$readmemh("programa.mem", rom); // Cambiar dirección del archivo si es necesario
//end

// Remplazar el $readmemh por datos fijos para que funcione en el Top
initial begin
	rom[0] = 32'h00510113; // Primera instrucción (PC = 0)
	rom[1] = 32'h00512223; // Segunda instrucción (PC = 4)
	rom[2] = 32'hFE000CE3; // Tercera instrucción (PC = 8)
	
	// Rellenamos algunas posiciones más para que el pipeline no lea 'X' cuando avance el PC
	rom[3] = 32'h00510113; 
	rom[4] = 32'h00512223;
	rom[5] = 32'hFE000CE3;
end

always@(*)
	RD=rom[A[31:2]];

endmodule
	
