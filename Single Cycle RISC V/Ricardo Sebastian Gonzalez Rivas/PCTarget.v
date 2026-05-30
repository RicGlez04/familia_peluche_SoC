module PCTarget(
    // Inputs
    input [31:0] PC,    // Program Counter actual
    input [31:0] ImmExt // Inmediato extendido (32 bits)

    // Outputs
    output [31:0] PCTarget 
);

// Asignar suma de ambos inputs
assign PCTarget = PC + ImmExt;

endmodule