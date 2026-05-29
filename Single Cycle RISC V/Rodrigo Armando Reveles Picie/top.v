module top(
input Clk, rst,
output reg out
);


wire PcSrcw;
wire [31:0]pc;
wire ImmSrcw;
wire Regwritew;
wire ALUSrcw;
wire [1:0]ALUOpw;
wire MemWritew;
wire [1:0]ResultSrcw;
wire PcSrc;

main_decoder CONTROL_UNIT(
.op(RDw[6:0]),
.zero(zerow),
.Regwrite(Regwritew),
.ImmSrc(ImmSrcw),
.ALUSrc(ALUSrcw),
.MemWrite(MemWritew),
.ResultSrc(ResultSrcw),
.PcSrc(PcSrcw),
.ALUOp(ALUOpw)
);

////////////////ALU DECODER/////////////////
wire [2:0]ALUControlw;
decoder_alu ALU_DECODER(
.op(RDw[6:0]),
.Aluop(ALUOpw),
.f3(RDw[14:12]	),
.f7(RDw[30]),
.control(ALUControlw)
);




/////////////PC_MODULE//////////////////////////////////
wire [31:0] Pcw;

pc_module PC(
    .clk(Clk),
    .reset(rst),
    .pc_next(pc),
    .pc(Pcw)        
);

wire [31:0] PcM; //pc counter actualizado a  pc + 4
//////////// ALU Del PC/////////////////
Alu_pc ALUpc(
.pc(Pcw),
.N_pc(PcM)
);


///////////MUX del PC
Mux2 Mux_PC(
 .control(PcSrcw),
.R(PcM),
.I(PCTw),
.out(pc)
);
///////////////////PC TARGET
wire [31:0] PCTw;
pc_target PC_target(
.PC(Pcw),
.Inm(immw),
.PCT(PCTw)

);

///////////////EXTEND/////////////////////

wire [31:0]immw;
extend EXTEND(
    .data(RDw[31:7]),
    .Immsurc(ImmSrcw),
    .imm(immw)
);

//////////////modulo registros//////////////////////
  wire [31:0] rd1w;
  wire [31:0] rd2w;
register_file registros(
	.clk(Clk),
   .we3(Regwritew),       // Write Enable (Señal de control para escribir)
   .a1(RDw[19:15]),        // Dirección de lectura 1 (rs1)
   .a2(RDw[24:20]),        // Dirección de lectura 2 (rs2)
   .a3(RDw[11:7]),        // Dirección de escritura (rd)
   .wd3(Resultw),       // Dato a escribir (Write Data)
   .rd1(rd1w),       // Dato leído 1 (Read Data 1)
   .rd2(rd2w)
);
////////////////MUX REGISTER
wire [31:0]MuxRw;
Mux2 Mux_register(
.control(ALUSrcw),
.R(rd2w),
.I(immw),
.out(MuxRw)
);
/////////////ALU MODULE///////////////////////
wire [31:0]ALU_resusltw;
wire zerow;

ALU ALU(
.A(rd1w), 
.B(MuxRw),
.ALU(ALUControlw),
.result(ALU_resusltw),
.zero(zerow)

);

/////////////////instrucion memory////////////////////

wire [31:0] RDw;
instruction_memory INST_MEMORY(
	.clk(Clk),
	.A(Pcw),
	.RD(RDw)
);

//////////////////////Data memory//////////////
wire [31:0] data_mem_out;
data_memory DATA_MEMORY(
    .clk(Clk),
    .WE(MemWritew),       
    .A(ALU_resusltw),   
    .WD(rd2w), 
    .RD(data_mem_out)     
);

/////////////////MUX DATA MEMORY/////////////////
wire [31:0]Rwmem;
wire [31:0]Resultw;

Mux2 MUX_DATAMEM(
.control(ResultSrcw),
.R(ALU_resusltw),
.I(data_mem_out),
.out(Resultw)
);




endmodule