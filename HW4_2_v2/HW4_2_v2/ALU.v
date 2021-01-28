module ALU(
    src1_i,
	src2_i,
	ctrl_i,
	result_o,
	zero_o
	);
     
//I/O ports
input  [64-1:0]  src1_i;
input  [64-1:0]	 src2_i;
input  [4-1:0]   ctrl_i;

output [64-1:0]	 result_o;
output           zero_o;

//Internal signals
reg    [64-1:0]  result_o;
wire             zero_o;

//Parameter

//Main function
assign zero_o = (src1_i == src2_i) ? 1 : 0; //BJ

always@* begin //BJ
    case(ctrl_i)
        4'b0000: result_o = src1_i & src2_i; // AND 
        4'b0001: result_o = src1_i | src2_i; // OR 
        4'b0010: result_o = src1_i + src2_i; // ADD 
		4'b0110: result_o = src1_i - src2_i; // SUB 
		4'b0111: result_o = (src1_i < src2_i) ? 1 : 0; // SLT 
		4'b1100: result_o = ~(src1_i | src2_i); // NOR 
		default: result_o = ctrl_i;
    endcase
end 
endmodule