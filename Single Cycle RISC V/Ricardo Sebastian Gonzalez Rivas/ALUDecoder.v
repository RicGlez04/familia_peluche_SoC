// El decodificador ALU se encarga de
// mandar la señal de control que necesita la ALU
// de acuerdo a la instrucción
module ALUDecoder(
    // Entradas
    input [1:0] ALUOp,           // Código op para la ALU
    input [2:0] funct3,
    input funct7,
    input op5,
    
    // Salidas
    output reg [2:0] ALUControl  // Señal de control para la ALU
);

// Dependiendo de la instrucción, la señal de control va a ser diferente
always @(*)
begin
    casex({ALUOp, funct3, op5, funct7}) // Código de instruccion concatenado
        7'b00xxxxx, 7'b1000000, 7'b1000001, 7'b1000010: ALUControl = 3'b000; // Add
        7'b01xxxxx, 7'b1000011: ALUControl = 3'b001;                         // Subtract
        7'b10010xx: ALUControl = 3'b101;                                     // Set less than
        7'b10110xx: ALUControl = 3'b011;                                     // Or
        7'b10111xx: ALUControl = 3'b010;                                     // And
    endcase
end

endmodule
