module InstructionMemory_tb();

reg [31:0] A;
wire [31:0] RD;

InstructionMemory DUT (
.A(A),
.RD(RD)
);

initial
	begin
		 // forzar unos datos en la ROM por si no lee archivo .mem
		 DUT.rom[0] = 32'h00510113; // Instruccion ficticia en la posición 0
		 DUT.rom[1] = 32'h00512223; // Instruccion ficticia en la posición 1
		 DUT.rom[2] = 32'hFE000CE3; // Instruccion ficticia en la posición 2
		 #5;
		 // leer la direccion 0 del PC
		 A = 0; // apunta a rom[0]
		 #10;

		 // leer la dirección 4 del PC, siguiente instruccion
		 A = 4; // apunta a rom[1]
		 #10;

		 // leer la dirección 8 del PC
		 A = 8; // apunta a rom[2]
		 #10;
	end

endmodule