module Control(
    instr_op_i,
	RegWrite_o,
	ALU_op_o,
	ALUSrc_o,
	Branch_o,
	MemWrite_o,
	MemRead_o,
	MemtoReg_o
	);
     
//I/O ports
input  [7-1:0] instr_op_i;

output         RegWrite_o;
output [2-1:0] ALU_op_o;
output         ALUSrc_o;
output         Branch_o;
output		   MemWrite_o;
output		   MemRead_o;
output		   MemtoReg_o;
 
//Internal Signals
reg    [2-1:0] ALU_op_o;
reg            ALUSrc_o;
reg            RegWrite_o;
reg            Branch_o;
reg			   MemWrite_o;
reg			   MemRead_o;
reg			   MemtoReg_o;

//Parameter


//Main function
always@* begin // BJ
    case(instr_op_i)
        7'b0110011: {ALUSrc_o,MemtoReg_o,RegWrite_o,MemRead_o,MemWrite_o,Branch_o,ALU_op_o} = 8'b00100010; // R format
		7'b0000011: {ALUSrc_o,MemtoReg_o,RegWrite_o,MemRead_o,MemWrite_o,Branch_o,ALU_op_o} = 8'b11110000; // ld
		7'b0100011: {ALUSrc_o,MemtoReg_o,RegWrite_o,MemRead_o,MemWrite_o,Branch_o,ALU_op_o} = 8'b1x001000; // sd
		7'b1100011: {ALUSrc_o,MemtoReg_o,RegWrite_o,MemRead_o,MemWrite_o,Branch_o,ALU_op_o} = 8'b0x000101; // beq
		7'b0010011: {ALUSrc_o,MemtoReg_o,RegWrite_o,MemRead_o,MemWrite_o,Branch_o,ALU_op_o} = 8'b10100011; // I format
		default: {ALUSrc_o,MemtoReg_o,RegWrite_o,MemRead_o,MemWrite_o,Branch_o,ALU_op_o} = 8'b00000000;
    endcase
end	
endmodule