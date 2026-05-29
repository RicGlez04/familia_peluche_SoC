module pc_module(
    input wire clk,
    input wire reset,
    input wire [31:0] pc_next,
    output reg [31:0] pc        
);

    // Bloque secuencial: se activa en el flanco de subida del reloj
    // o en el flanco de subida del reset (reset asíncrono)
    always @(posedge clk or posedge reset) begin
        if (reset) begin
            // si hay reset el pc vuelve a la dirección inicial
            pc <= 32'b0; 
        end 
		  else 
		  begin
            pc <= pc_next;
        end
    end

endmodule