// Función: Registro entre etapas Fetch y Decode.
// Las señales pasan de F a D.
module Register_IF_ID(
	input clk, rst, StallD, FlushD,				// Se incluyen señales de peligro StallD y FlushD
	input [31:0] RD, PC, PCPlus4F,
	output reg [31:0] InstrD, PCD, PCPlus4D
);

// Control de datos con clk o rst
always@(posedge clk or posedge rst)
	begin
		// Reinicio de datos
		if(rst)
			begin
				InstrD<=0; 
				PCD<=0;
				PCPlus4D<=0;
			end
		// Limpiar datos con FlushD
		else if(FlushD)
			begin
				InstrD<=0; 
				PCD<=0;
				PCPlus4D<=0;
			end
		// Mantener datos con StallD
		else if(StallD)
			begin
				InstrD<=InstrD; 
				PCD<=PCD;
				PCPlus4D<=PCPlus4D;
			end
		// Paso de datos con clk
		else
			begin
				InstrD<=RD; 
				PCD<=PC;
				PCPlus4D<=PCPlus4F;
			end
			
	end
endmodule	
