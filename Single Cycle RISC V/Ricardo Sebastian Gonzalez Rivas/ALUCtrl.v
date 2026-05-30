// Este modulo representa el ALU en si
module ALUCtrl #(parameter N = 12) (
    input [N-1:0] A,
    input [N-1:0] B,
    input [2:0] ALUControl,
    output reg Zero,
    output reg [N-1:0] result
);

always @(*)
begin
    case(ALUControl)
        3'b000: result = A + B; // Add
        3'b001: result = A - B; // Subtract
        3'b010: result = A & B; // And
        3'b011: result = A | B; // Or
        3'b101: // Also A << B // SLT (Set on less than) 
            begin
                if (A < B)
                    result = 1;
                else result = 0;
            end
        default: result = 0;
    endcase

    if (result == 0)
        Zero = 1;
    else
        Zero = result;
end


endmodule