module DataMemory_tb();

reg [31:0] Alu_result, WriteData;
reg MemWrite, clk, rst;
wire [31:0] ReadData;

DataMemory DUT (
.Alu_result(Alu_result),
.WriteData(WriteData),
.MemWrite(MemWrite),
.clk(clk),
.rst(rst),
.ReadData(ReadData)
);

always
	begin
		 clk = 0; #5;
		 clk = 1; #5;
	end

initial
	begin
		// limpiar la memoria con rst
		rst = 1;
		MemWrite = 0;
		Alu_result = 0;
		WriteData = 0;
		#10; // ciclo completo de reloj
		rst = 0; // desactivar rst
		#5;

		// Escribir el dato 0xAAAA_BBBB en la direccion 4
		Alu_result = 4; // usa la posición [4 >> 2] = 1 de la RAM
		WriteData = 32'hAAAA_BBBB;
		MemWrite = 1; // habilitar write
		#10; // esperar el flanco del reloj

		// Escribir el dato 0x5555_6666 en la dirección 8
		Alu_result = 8; // posición [8 >> 2] = 2 de la RAM
		WriteData = 32'h5555_6666;
		MemWrite = 1;
		#10;

		// lectura asincrona de la dirección 4
		MemWrite = 0; // para solo leer
		Alu_result = 4; // ReadData cambia a 0xAAAA_BBBB
		#10;

		// Lectura asincrona de la dirección 8
		Alu_result = 8; // ReadData cambia a 0x5555_6666
		#10;
	end
	
endmodule