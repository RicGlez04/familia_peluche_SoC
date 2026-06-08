module top(
    input Clk, rst
);

// INTERCONEXIÓN ENTRE ETAPAS (F, D, E, M, W)

	// Etapa F (Fetch)
	wire [31:0] pc_next, pcF, InstrF, PCPlus4F;
	
	// Etapa D (Decode)
	wire [31:0] InstrD, pcD, PCPlus4D, rd1D, rd2D, immD;
	wire [4:0] rdD, rs1D, rs2D;
	wire RegWriteD, MemWriteD, JumpD, BranchD, AluSrcD;
	wire [1:0] ImmSrcD, ResultSrcD;
	wire [2:0] AluControlD;

	// Etapa E (Execute)
	wire RegWriteE, MemWriteE, JumpE, BranchE, AluSrcE;
	wire [1:0] ResultSrcE, ForwardAE, ForwardBE;
	wire [2:0] AluControlE;
	wire [31:0] pcE, rd1E, rd2E, immE, PCPlus4E;
	wire [4:0] rs1E, rs2E, rdE;
	wire [31:0] SrcAE, SrcBE, WriteDataE, ALU_resultE, PCTargetE;
	wire zeroE, PCSrcE;

	// Etapa M (Memory)
	wire RegWriteM, MemWriteM;
	wire [1:0] ResultSrcM;
	wire [31:0] ALU_resultM, WriteDataM, ReadDataM, PCPlus4M;
	wire [4:0] rdM;

	// Etapa W (Writeback)
	wire RegWriteW;
	wire [1:0] ResultSrcW;
	wire [31:0] ALU_resultW, ReadDataW, PCPlus4W, ResultW;
	wire [4:0] rdW;

	// Señales de Control de Riesgos (Hazard Unit)
	wire StallF, StallD, FlushD, FlushE;

// ETAPA 1: FETCH (F)
    
	// Mux para el siguiente PC (Utiliza tu módulo Mux con control de 2 bits o asignación directa)
	assign pc_next = PCSrcE ? PCTargetE : PCPlus4F;

	ProgramCounter PC_MODULE (
	.clk(Clk),
	.rst(rst),
	.StallF(StallF),
	.PCNext(pc_next),
	.PC(pcF)
	);

	InstructionMemory INST_MEMORY (
	.A(pcF),
	.RD(InstrF)
	);

	Adder Adder_PC_Plus4 (
	.PC(pcF),
	.variable(32'd4),
	.AdderOut(PCPlus4F)
	);

	// REGISTRO DE ETAPA: IF -> ID

	Register_IF_ID REG_IF_ID (
	.clk(Clk), .rst(rst), .StallD(StallD), .FlushD(FlushD),
	.RD(InstrF), .PC(pcF), .PCPlus4F(PCPlus4F),
	.InstrD(InstrD), .PCD(pcD), .PCPlus4D(PCPlus4D)
	);

// ETAPA 2: DECODE (D)

	assign rdD  = InstrD[11:7];
	assign rs1D = InstrD[19:15];
	assign rs2D = InstrD[24:20];

	ControlUnit CONTROL_UNIT (
	.op(InstrD[6:0]),
	.funct3(InstrD[14:12]),
	.funct7(InstrD[30]),
	.AluControlD(AluControlD),
	.MemWriteD(MemWriteD),
	.AluSrcD(AluSrcD),
	.RegWriteD(RegWriteD),
	.JumpD(JumpD),
	.BranchD(BranchD),
	.ImmSrcD(ImmSrcD),
	.ResultSrcD(ResultSrcD)
	);

	Register_File registros (
	.clk(Clk),
	.rst(rst),
	.WE3(RegWriteW),
	.A1(rs1D),
	.A2(rs2D),
	.A3(rdW),
	.WD3(ResultW),
	.RD1(rd1D),
	.RD2(rd2D)
	);

	Imm_Extend EXTEND (
	.ImmSrc(ImmSrcD),
	.Instr(InstrD),
	.ImmExt(immD)
	);


    // REGISTRO DE ETAPA: ID -> EX

	Register_ID_EX REG_ID_EX (
	.clk(Clk), .rst(rst), .FlushE(FlushE),
	.MemWriteD(MemWriteD), .AluSrcD(AluSrcD), .RegWriteD(RegWriteD), .JumpD(JumpD), .BranchD(BranchD),
	.ResultSrcD(ResultSrcD), .AluControlD(AluControlD),
	.PCD(pcD), .ExtImmD(immD), .PCPlus4D(PCPlus4D), .RD1(rd1D), .RD2(rd2D),
	.Rs1D(rs1D), .Rs2D(rs2D), .RdD(rdD),
	.MemWriteE(MemWriteE), .AluSrcE(AluSrcE), .RegWriteE(RegWriteE), .JumpE(JumpE), .BranchE(BranchE),
	.ResultSrcE(ResultSrcE), .AluControlE(AluControlE),
	.PCE(pcE), .ExtImmE(immE), .PCPlus4E(PCPlus4E), .RD1E(rd1E), .RD2E(rd2E),
	.Rs1E(rs1E), .Rs2E(rs2E), .RdE(rdE)
	);


// ETAPA 3: EXECUTE (E)
    
	// Muxes de reenvío (Forwarding) de 4 a 1 usando tu módulo Mux.v
	Mux Mux_ForwardA (
	.A(rd1E), .B(ResultW), .C(ALU_resultM), .D(32'b0),
	.control(ForwardAE), .OutMux(SrcAE)
	);

	Mux Mux_ForwardB (
	.A(rd2E), .B(ResultW), .C(ALU_resultM), .D(32'b0),
	.control(ForwardBE), .OutMux(WriteDataE)
	);

	// Mux para la entrada B de la ALU (Selección de Inmediato)
	// Reutiliza tu módulo Mux configurando control en 1 bit (0: Registro, 1: Inmediato)
	Mux Mux_ALUSrc (
	.A(WriteDataE), .B(immE), .C(32'b0), .D(32'b0),
	.control({1'b0, AluSrcE}),
	.OutMux(SrcBE)
	);

	Alu ALU (
	.An(SrcAE),
	.Bn(SrcBE),
	.AluControl(AluControlE),
	.Alu_result(ALU_resultE),
	.ZeroE(zeroE)
	);

	Adder Adder_PC_Target (
	.PC(pcE),
	.variable(immE),
	.AdderOut(PCTargetE)
	);

	BranchComparator BranchCond (
	.Branch(BranchE),
	.Zero(zeroE),
	.Jump(JumpE),
	.PCSrc(PCSrcE)
	);

// REGISTRO DE ETAPA: EX -> MEM

	Register_EX_MEM REG_EX_MEM (
	.clk(Clk), .rst(rst), .MemWriteE(MemWriteE), .RegWriteE(RegWriteE),
	.ResultSrcE(ResultSrcE), .Alu_result(ALU_resultE), .WriteDataE(WriteDataE), .PCPlus4E(PCPlus4E),
	.RdE(rdE),
	.MemWriteM(MemWriteM), .RegWriteM(RegWriteM),
	.ResultSrcM(ResultSrcM), .Alu_resultM(ALU_resultM), .WriteDataM(WriteDataM), .PCPlus4M(PCPlus4M),
	.RdM(rdM)
	);


// ETAPA 4: MEMORY (M)

    DataMemory DATA_MEMORY (
        .Alu_result(ALU_resultM),
        .WriteData(WriteDataM),
        .MemWrite(MemWriteM),
        .clk(Clk),
        .rst(rst),
        .ReadData(ReadDataM)
    );


// REGISTRO DE ETAPA: MEM -> WB
	 
	Register_MEM_WB REG_MEM_WB (
	.clk(Clk), .rst(rst), .RegWriteM(RegWriteM),
	.ResultSrcM(ResultSrcM), .Alu_resultM(ALU_resultM), .ReadData(ReadDataM), .PCPlus4M(PCPlus4M),
	.RdM(rdM),
	.RegWriteW(RegWriteW), .ResultSrcW(ResultSrcW),
	.Alu_resultW(ALU_resultW), .ReadDataW(ReadDataW), .PCPlus4W(PCPlus4W),
	.RdW(rdW)
	);


// ETAPA 5: WRITEBACK (W)
		 
	// Mux de 4 a 1 final para decidir qué se escribe en los registros (ALU, RAM o PC+4)
	Mux Mux_Writeback (
	.A(ALU_resultW), .B(ReadDataW), .C(PCPlus4W), .D(32'b0),
	.control(ResultSrcW),
	.OutMux(ResultW)
	);

// 6. UNIDAD DE RIESGOS (HAZARD UNIT) - Sincroniza todo el Pipeline

	Hazard_Unit HazardController (
	.PCSrcE(PCSrcE),
	.ResultSrcE0(ResultSrcE[0]),
	.RegWriteM(RegWriteM),
	.RegWriteW(RegWriteW),
	.Rs1D(rs1D), .Rs2D(rs2D), .RdE(rdE), .Rs2E(rs2E), .Rs1E(rs1E), .RdM(rdM), .RdW(rdW),
	.StallF(StallF), .StallD(StallD), .FlushD(FlushD), .FlushE(FlushE),
	.ForwardAE(ForwardAE), .ForwardBE(ForwardBE)
	);

endmodule