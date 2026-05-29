module ALU_tb();

reg [31:0] a,b;
reg [2:0] alu;
wire [31:0] r;
wire zero;

ALU TB(
.a(A),
.b(B),
.alu(ALU),
.r(result),
.z(zero)
);

initial
begin

a = 5;
b = 6;
#10;

alu = 0;

#10;


end





endmodule