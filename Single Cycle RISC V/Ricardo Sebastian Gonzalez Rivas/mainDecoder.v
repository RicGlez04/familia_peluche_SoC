// Este modulo implementa el main decoder que va dentro del Control Unit
module mainDecoder(
    // Entradas
    input [6:0] op,

    // Salidas
    output reg Branch,
    output reg [1:0] ResultSrc, 
    output reg MemWrite,
    output reg ALUSrc,
    output reg [1:0] ImmSrc,
    output reg RegWrite,
    output reg [1:0] ALUOp
);

// Las señales de control cambian dependiendo del op
always @(*)
begin
    casex(op)
        7'd3: begin // lw
            RegWrite = 1;
            ImmSrc = 00;
            ALUSrc = 1;
            MemWrite = 0;
            ResultSrc = 01;
            Branch = 0;
            ALUOp = 00;
        end
        7'd35: begin // sw
            RegWrite = 0;
            ImmSrc = 01;
            ALUSrc = 1;
            MemWrite = 1;
            ResultSrc = 2'bxx;
            Branch = 0;
            ALUOp = 00; 
        end
        7'd51: begin // R-type
            RegWrite = 1;
            ImmSrc = 2'bxx;
            ALUSrc = 0;
            MemWrite = 0;
            ResultSrc = 00;
            Branch = 0;
            ALUOp = 10; 
        end
        7'd99: begin // beq
            RegWrite = 0;
            ImmSrc = 10;
            ALUSrc = 0;
            MemWrite = 0;
            ResultSrc = 2'bxx;
            Branch = 1;
            ALUOp = 01; 
        end
        7'd19: begin // l-type
            RegWrite = 1;
            ImmSrc = 00;
            ALUSrc = 1;
            MemWrite = 0;
            ResultSrc = 00;
            Branch = 0;
            ALUOp = 10; 
        end
        default: begin
            RegWrite = 0;
            ImmSrc = 00;
            ALUSrc = 0;
            MemWrite = 0;
            ResultSrc = 00;
            Branch = 0;
            ALUOp = 00;
        end
    endcase 
end


endmodule
