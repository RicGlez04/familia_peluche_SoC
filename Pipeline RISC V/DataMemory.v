// Función: Memoria RAM del procesador para leer y escribir datos dinámicos.
module DataMemory(
	input [31:0] Alu_result, WriteData,
	input MemWrite, clk, rst,
	output reg [31:0] ReadData
);

reg[31:0] memoria [0:63];
integer i;

always@(posedge clk or posedge rst)
	begin
		if (rst) 
			begin
				for (i = 0; i < 64; i = i + 1) 
					memoria[i] <= 32'h0;
			end
		
		else if(MemWrite==1)
			memoria[Alu_result[31:2]]<=WriteData;
	end

always@(*)
	ReadData=memoria[Alu_result[31:2]];

endmodule