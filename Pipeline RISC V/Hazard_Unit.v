module Hazard_Unit(
	input PCSrcE, ResultSrcE0, RegWriteM, RegWriteW,
	input [4:0] Rs1D, Rs2D, RdE, Rs2E, Rs1E, RdM, RdW,
	output reg StallF, StallD, FlushD, FlushE, 
	output reg [1:0] ForwardAE, ForwardBE
);

// Señal intermedia
wire lwStall;

always @(*)
	begin
		// Protocolo de forwarding para ambos Src1 y Src2
		if (((Rs1E == RdM) && RegWriteM) && (Rs1E != 0))
			ForwardAE = 2'b10;
		else if (((Rs1E == RdW) && RegWriteW) && (Rs1E != 0))
			ForwardAE = 2'b01;
		else 		
			ForwardAE = 2'b00;

		if (((Rs2E == RdM) && RegWriteM) && (Rs2E != 0))
			ForwardBE = 2'b10;
		else if (((Rs2E == RdW) && RegWriteW) && (Rs2E != 0))
			ForwardBE = 2'b01;
		else 		
			ForwardBE = 2'b00;

		// Protocolo de Stall y Flush
		StallF = lwStall;
		StallD = lwStall;
		FlushD = PCSrcE;
		FlushE = lwStall | PCSrcE;
	end

assign lwStall = ((Rs1D == RdE) | (Rs2D == RdE)) && ResultSrcE0;

endmodule
