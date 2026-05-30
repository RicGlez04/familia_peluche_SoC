module PCPlus4(
    // Input
    input [31:0] PC,

    // Output
    output [31:0] PCPlus4,
);

// Añadir cuatro al PC
assign PCPlus4 = PC + 32'd4;

endmodule