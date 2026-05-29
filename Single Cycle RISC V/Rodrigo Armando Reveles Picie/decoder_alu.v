module decoder_alu(
input op,
input [1:0]Aluop,
input [2:0]f3,
input f7,
output reg [2:0]control
);

wire [6:0] inst;
// modulo que forma parte del control unit pero se encarga de cambiar la señal de ALUControl


assign inst = {Aluop,f3,op,f7};
always@(*)
begin
casex(inst)
7'b00xxxxx: control =3'b000;
7'b01xxxxx: control =3'b001;
7'b1000000: control =3'b000;
7'b1000001: control =3'b000;
7'b1000010: control =3'b000;
7'b1000011: control =3'b001;
7'b10010xx: control =3'b101;
7'b10110xx: control =3'b011;
7'b10111xx: control =3'b010;


endcase
end





endmodule