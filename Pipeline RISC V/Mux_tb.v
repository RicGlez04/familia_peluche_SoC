module Mux_tb();

reg [31:0] A, B, C, D;
reg [1:0] control;
wire [31:0] OutMux;

Mux DUT (
.A(A),
.B(B),
.C(C),
.D(D),
.control(control),
.OutMux(OutMux)
);

initial
	begin
		// valores fijos a las entradas
		A = 32'hAAAA_AAAA;
		B = 32'hBBBB_BBBB;
		C = 32'hCCCC_CCCC;
		D = 32'hDDDD_DDDD;
		
		control = 0;
		#10; // OutMux A

		// entrada B
		control = 1;
		#10; // OutMux B

		// entrada C
		control = 2;
		#10; // OutMux C

		// entrada D
		control = 3;
		#10; // OutMux D
	end

endmodule