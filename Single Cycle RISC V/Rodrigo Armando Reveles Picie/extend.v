module extend(
    input [31:7] data,
    input [1:0] Immsurc,
    output reg [31:0] imm
);

//este modulo es el de extensor de bit toma los bits de inmediato dependiendo del dipo de instruccion
// porque se codifica diferente  

always @(*)
begin
    casex(Immsurc)
        2'b00: imm = { {20{data[31]}}, data[31:20] };
        2'b01: imm = { {20{data[31]}}, data[31:25], data[11:7] };
        2'b10: imm = { {19{data[31]}}, data[31], data[7], data[30:25], data[11:8], 1'b0 };
        2'b11: imm = { {12{data[31]}}, data[19:12], data[20], data[30:21], 1'b0 };
        
        default: imm = 32'b0;
    endcase
end

endmodule