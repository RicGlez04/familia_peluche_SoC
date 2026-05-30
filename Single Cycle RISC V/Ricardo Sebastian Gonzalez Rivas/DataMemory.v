module DataMemory(
    // Inputs
    input clk,
    input WE,
    input [31:0] A,
    input [31:0] WD,
    
    // Outputs
    output [31:0] RD
);
    reg [31:0] RAM [63:0]; // Memoria pequeña para simulación

    always @(posedge clk) begin
        if (WE)
            RAM[A[31:2]] <= WD;
    end

    assign RD = RAM[A[31:2]];
endmodule