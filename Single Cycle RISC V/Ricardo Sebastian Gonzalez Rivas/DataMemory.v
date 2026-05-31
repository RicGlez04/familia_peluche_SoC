module DataMemory(
    // Entradas
    input clk,
    input WE,         // Señal de control en caso de requerir escritura
    input [31:0] A,
    input [31:0] WD,
    
    // Salidas
    output [31:0] RD
);

// Memoria pequeña para simulación
reg [31:0] RAM [63:0]; 

// 
always @(posedge clk) begin
    if (WE)
        RAM[A[31:2]] <= WD;
end

assign RD = RAM[A[31:2]];


endmodule
