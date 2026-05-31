// Este módulo se encarga de las operaciones con la ALU principal
// utilizando las señales de salida de la Register File
module ALUCtrl #(parameter N = 12) (
    // Entradas
    input [N-1:0] A,
    input [N-1:0] B,
    input [2:0] ALUControl,     // Señal de control para la ALU
    
    // Salidas
    output reg Zero,            // Señal booleana en caso de instrucción tipo B
    output reg [N-1:0] result
);

// La operación depende de la señal de control
always @(*)
begin
    case(ALUControl)
        3'b000: result = A + B;   // Add
        3'b001: result = A - B;   // Subtract
        3'b010: result = A & B;   // And
        3'b011: result = A | B;   // Or
        3'b101: // Also A << B    // SLT (Set on less than) 
            begin
                if (A < B)
                    result = 1;
                else result = 0;
            end
    endcase

    // Lógica para señal Zero
    if (result == 0)
        Zero = 1;
    else
        Zero = result;
end


endmodule
