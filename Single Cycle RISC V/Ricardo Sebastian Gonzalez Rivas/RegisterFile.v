module RegisterFile(
    // Entradas
    input clk,
    input WE3,          // Se activa al escribir un registro
    input [4:0] A1,     // Register Source 1
    input [4:0] A2,     // Register Source 2
    input [4:0] A3,     // Register Destination
    input [31:0] WD3,   // Escritura de registro
    
    // Salidas
    output [31:0] RD1,
    output [31:0] RD2
);
// Son 32 registros de 32 bits
reg [31:0] RF [31:0];

// Escritura de registros
always @(posedge clk) begin
    if (WE3)
        RF[A3] <= WD3;
end

// Lógica para registros de salida (solo cuando no son igual a cero)
assign RD1 = (A1 == 5'b0) ? 32'b0 : RF[A1];
assign RD2 = (A2 == 5'b0) ? 32'b0 : RF[A2];

endmodule
