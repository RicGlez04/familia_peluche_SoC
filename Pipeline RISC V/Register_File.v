// Función: Banco de 32 registros internos para almacenamiento de variables y datos temporales.
module Register_File(
		input[4:0] A1, A2, A3,
		input [31:0] WD3, 
		input WE3, clk, rst,
		output reg [31:0] RD1, RD2
);

// 32 registros de 32 bits
reg[31:0] registers[0:31];

// Variable como iterador	
integer i;

// Escritura con ~clk o rst
always@(negedge clk or posedge rst)
	begin
		if (rst) 
			begin
				for (i = 0; i < 32; i = i + 1) 
                registers[i] <= 32'h0;
         end
		else if(WE3==1 && A3!=0)
			registers[A3]<=WD3;
	end

// Control de RD1 asincrona
always@(*)
	begin
		if(A1==0)
			RD1=0;
		else
			RD1=registers[A1];
	end

// Control de RD2 asincrona
always@(*)
	begin
		if(A2==0)
			RD2=0;
		else
			RD2=registers[A2];
	end
endmodule
