module InstructionMemory(
	input clk,
	input [31:0] A,        // Instruction memory address
	
	output [31:0] RD   // Destination register
);

	reg [31:0] instr_mem [0:2]; // 0:10_000
	
	initial begin
		$readmemh("instrMem.hex",instr_mem);
	end
	
	assign RD = instr_mem[A[31:2]];

endmodule
