module PCPlus4(
    // Entradas
    input [31:0] PC,

    // Salidas
    output [31:0] PCPlus4  // PC sumado con 4
);

// Añadir cuatro al PC
assign PCPlus4 = PC + 32'd4;

endmodule
