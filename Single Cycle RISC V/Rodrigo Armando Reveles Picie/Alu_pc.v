module Alu_pc(
input wire [31:0]pc,
output reg [31:0]N_pc
);
// este modulo le egrega 4 al pc counter para que pueda incrementar
always@(*)
begin
	N_pc = pc + 4;
end

endmodule