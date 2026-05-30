module riscv_single_cycle(
    input clk,
    input rst
);
    // Wires internos de interconexión
    wire [31:0] PCNext_wire, PC_wire, PCPlus4_wire, PCTarget_wire;
    wire [31:0] Instr;
    wire [31:0] Result;
    wire [31:0] RD1, RD2, SrcB, ALUResult, ReadData, ImmExt;
    
    // Señales de Control
    wire PCSrc, MemWrite, ALUSrc, RegWrite;
    wire [1:0] ResultSrc;
    wire [1:0] ImmSrc;
    wire [2:0] ALUControl;
    wire Zero;

    // --- LÓGICA DEL PROGRAM COUNTER ---
    PCNext pc_mux (
        .PCSrc(PCSrc),
        .PCPlus4(PCPlus4_wire),
        .PCTarget(PCTarget_wire),
        .PCNext(PCNext_wire)
    );

    registro pc_reg (
        .clk(clk),
        .rst(rst),
        .PCin(PCNext_wire),
        .PCout(PC_wire)
    );

    PCPlus4 pc_add4 (
        .PC(PC_wire),
        .PCPlus4(PCPlus4_wire)
    );

    PCTarget pc_add_target (
        .PC(PC_wire),
        .ImmExt(ImmExt),
        .PCTarget(PCTarget_wire)
    );

    // --- MEMORIA DE INSTRUCCIONES ---
    InstructionMemory instr_mem (
        .clk(clk),
        .A(PC_wire),
        .RD(Instr)
    );

    // --- UNIDAD DE CONTROL ---
    // Nota: Adaptamos las salidas para que coincidan con los nombres correctos
    ControlUnit control_unit (
        .op(Instr[6:0]),
        .funct3(Instr[14:12]),
        .funct7(Instr[30]),
        .zero(Zero),
        .PCSrc(PCSrc),
        .ResultSrc(ResultSrc), // Ahora es de 2 bits
        .MemWrite(MemWrite),
        .ALUControl(ALUControl),
        .ALUSrc(ALUSrc),
        .ImmSrc(ImmSrc),
        .RegWrite(RegWrite)
    );

    // --- BANCO DE REGISTROS ---
    RegisterFile reg_file (
        .clk(clk),
        .WE3(RegWrite),
        .A1(Instr[19:15]),
        .A2(Instr[24:20]),
        .A3(Instr[11:7]),
        .WD3(Result),
        .RD1(RD1),
        .RD2(RD2)
    );

    // --- EXTENSOR DE INMEDIATOS ---
    Extender ext (
        .ImmSrc(ImmSrc),
        .Instruction(Instr),
        .ImmExt(ImmExt)
    );

    // --- MULTIPLEXOR ALU SRC B ---
    ALUSrc alu_src_b_mux (
        .ALUSrc(ALUSrc),
        .RD2(RD2),
        .ImmExt(ImmExt),
        .SrcB(SrcB)
    );

    // --- ALU ---
    // Ajustado el parámetro N a 32 para consistencia de datos
    ALUCtrl #(.N(32)) alu (
        .A(RD1),
        .B(SrcB),
        .ALUControl(ALUControl),
        .Zero(Zero),
        .result(ALUResult)
    );

    // --- MEMORIA DE DATOS ---
    DataMemory data_mem (
        .clk(clk),
        .WE(MemWrite),
        .A(ALUResult),
        .WD(RD2), // WriteData en el diagrama es RD2
        .RD(ReadData)
    );

    // --- MULTIPLEXOR DE RESULTADO FINAL ---
    result result_mux (
        .ResultSrc(ResultSrc),
        .ALUResult(ALUResult),
        .ReadData(ReadData),
        .PCPlus4(PCPlus4_wire),
        .Result(Result)
    );

endmodule