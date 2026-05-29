module register_file (
    input  wire        clk,
    input  wire        we3,       // Write Enable (Señal de control para escribir)
    input  wire [4:0]  a1,        // Dirección de lectura 1 (rs1)
    input  wire [4:0]  a2,        // Dirección de lectura 2 (rs2)
    input  wire [4:0]  a3,        // Dirección de escritura (rd)
    input  wire [31:0] wd3,       // Dato a escribir (Write Data)
    output reg [31:0] rd1,       // Dato leído 1 (Read Data 1)
    output reg [31:0] rd2        // Dato leído 2 (Read Data 2)
);

    // Declaración de la memoria: 32 registros de 32 bits de ancho
    reg [31:0] registers [31:0];

    // 1. LECTURA (Asíncrona / Combinacional)
	 
  always @(*) begin
        // Lectura del puerto 1 (rd1)
        if (a1 == 5'b00000) begin
            rd1 = 32'b0;
        end else begin
            rd1 = registers[a1];
        end

        // Lectura del puerto 2 (rd2)
        if (a2 == 5'b00000) begin
            rd2 = 32'b0;
        end else begin
            rd2 = registers[a2];
        end
    end

    // escritura (Sincrona)
    always @(posedge clk) begin
        if (we3) begin
            // Protegemos el registro 0: solo escribimos si a3 es diferente de 0
            if (a3 != 5'b00000) begin
                registers[a3] <= wd3;
            end
        end
    end

endmodule