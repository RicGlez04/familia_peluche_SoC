module data_memory (
    input clk,
    input  WE,       
    input [31:0]A,   
    input  [31:0]WD, 
    output wire [31:0] RD     
);

    // Declaración de la memoria: 32 espacios de  32 bits de ancho
    reg [31:0] Memory [31:0];
	
	assign RD = Memory[A[31:2]];
  
	 
    // 2. ESCRITURA (Síncrona)
    // --------------------------------------------------------
    // Solo escribimos cuando hay un flanco de subida en el reloj Y si WE es 1.
    always @(posedge clk) begin
        if (WE) begin
            Memory[A[31:2]] <= WD;
        end
    end

endmodule