module Mux2(
input control,
input[31:0] R,I,
output reg out
);

//un multiplexor de 2 a 1 que tiene varias aplicaciones en la arquitectura
always@(*)
begin
	case(control)
	1'b0: out =R;
	1'b1: out =I;

	default: out = 0;
	endcase

end

endmodule