module ALU(
input [31:0] A, B,
input [2:0] ALU,
output reg [31:0]result,
output reg zero
);


always@(*)
	begin
		case(ALU)
	  3'b000:begin
	  
	  if((A+B)==0)begin
	  zero = 1;
	  result = A+B;
	  end
	  
	  else
	  begin
	  result = A+B;
	  end
	  end
	  
	  3'b001:begin 
	  if((A-B)==0)begin
	  zero = 1;
	  result = A-B;
	  end
	  else
	  begin
	  result = A-B;
	  end
	  end
	  3'b010: begin
	  if((A&B)==0)begin
	  zero = 1;
	  result = A&B;
	  end
	  else
	  begin
	  result = A&B;
	  end
	  
	  end
	  3'b011:begin 
	  if((A|B)==0)begin
	  zero = 1;
	  result = A|B;
	  end
	  
	  else
	  begin
	  result = A|B;
	  end
	  end
	  3'b101: 
	  
	  if((A<<B)==0)begin
	  zero = 1;
	  result = A<<B;
	  end
	  
	  else
	  begin
	  result = A<<B;
	  end
	  
	  default: result =0;
		endcase 

	end
	
	
	endmodule