module pc_target(
input [31:0]PC,
input [31:0]Inm,
output reg [31:0]PCT
);

always@(*)
begin
PCT = PC + Inm;
end

endmodule