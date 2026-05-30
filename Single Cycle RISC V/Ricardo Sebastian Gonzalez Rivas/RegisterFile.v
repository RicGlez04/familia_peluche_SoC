module RegisterFile(
    input clk,
    input WE3,
    input [4:0] A1,
    input [4:0] A2,
    input [4:0] A3,
    input [31:0] WD3,
    output [31:0] RD1,
    output [31:0] RD2
);
    reg [31:0] rf [31:0];

    // Escritura síncrona (en flanco de bajada o subida, usualmente bajada en ciclo único para evitar hazards)
    always @(posedge clk) begin
        if (WE3 && (A3 != 5'b0))
            rf[A3] <= WD3;
    end

    // Lectura asíncrona combinacional
    assign RD1 = (A1 == 5'b0) ? 32'b0 : rf[A1];
    assign RD2 = (A2 == 5'b0) ? 32'b0 : rf[A2];

endmodule