module Hazard_Unit_tb();

reg PCSrcE, ResultSrcE0, RegWriteM, RegWriteW;
reg [4:0] Rs1D, Rs2D, RdE, Rs2E, Rs1E, RdM, RdW;

wire StallF, StallD, FlushD, FlushE;
wire [1:0] ForwardAE, ForwardBE;

Hazard_Unit DUT (
.PCSrcE(PCSrcE),
.ResultSrcE0(ResultSrcE0),
.RegWriteM(RegWriteM),
.RegWriteW(RegWriteW),
.Rs1D(Rs1D),
.Rs2D(Rs2D),
.RdE(RdE),
.Rs2E(Rs2E),
.Rs1E(Rs1E),
.RdM(RdM),
.RdW(RdW),
.StallF(StallF),
.StallD(StallD),
.FlushD(FlushD),
.FlushE(FlushE),
.ForwardAE(ForwardAE),
.ForwardBE(ForwardBE)
);

initial
	begin
		// sin hazards
		PCSrcE = 0; ResultSrcE0 = 0; RegWriteM = 0; RegWriteW = 0;
		Rs1D = 1; Rs2D = 2; RdE = 3; Rs1E = 1; Rs2E = 2; RdM = 4; RdW = 5;
		#10; // Forward=00, Stalls=0, Flushes=0

		// Forwarding desde la etapa Memory (M) hacia Execute (E)
		// El destino en M (RdM = 5) coincide con la fuente en E (Rs1E = 5) y RegWriteM está activo
		RegWriteM = 1;
		Rs1E = 5; 
		#10; // Esperando ForwardAE = 2'b10

		// Forwarding desde la etapa WriteBack (W) hacia Execute (E)
		// El destino en W (RdW = 6) coincide con la fuente en E (Rs2E = 6) y RegWriteW está activo
		RegWriteM = 0; // apagar el hazard anterior
		RegWriteW = 1;
		Rs2E = 6;
		RdW = 6;
		#10; // Esperando ForwardBE = 2'b01

		// Stall por dependencia de un Load (lwStall)
		// Instrucción en Decode (Rs1D o Rs2D) depende del destino en Execute (RdE)
		// y la instrucción en Execute es un Load (ResultSrcE0 = 1)
		RegWriteW = 0;
		ResultSrcE0 = 1; // instruccion en Execute lee de la memoria
		Rs1D = 8; // instruccion quiere usar el registro 8
		RdE = 8; // Load trae el dato para el registro 8
		#10; // Esperado lwStall activo = StallF=1, StallD=1, FlushE=1

		// Control Hazard (Salto tomado / Flush)
		// si hay un salto en Execute (PCSrcE = 1), se deben limpiar las instrucciones de Decode y Execute
		ResultSrcE0 = 0; // quitar el Stall anterior
		PCSrcE = 1; // salto
		#10; // Esperado FlushD=1, FlushE=1
	end

endmodule