module ALU_Ctrl(
          funct_i,
          ALUOp_i,
          ALUCtrl_o
          );
          
//I/O ports 
input      [4-1:0] funct_i;
input      [2-1:0] ALUOp_i;

output     [4-1:0] ALUCtrl_o;    
     
//Internal Signals
reg        [4-1:0] ALUCtrl_o;

//Parameter


//Select exact operation, please finish the following code
always@(funct_i or ALUOp_i) begin
    case(ALUOp_i)
        2'b10: // R format BJ
            begin
                case(funct_i)
                    4'b0111: ALUCtrl_o = 4'b0000; // AND
                    4'b0000: ALUCtrl_o = 4'b0010; // ADD BJ
                    4'b1000: ALUCtrl_o = 4'b0110; // SUB BJ
                    4'b0110: ALUCtrl_o = 4'b0001; // OR BJ
                    4'b0010: ALUCtrl_o = 4'b0111; // SLT BJ
                    default: ALUCtrl_o = 4'b1111;
                endcase
            end
        2'b00: ALUCtrl_o = 4'b0010; // ld sd  use add BJ
        2'b01: ALUCtrl_o = 4'b0110; // beq use sub BJ
        2'b11: // I format BJ
            begin
                case(funct_i[2:0])
                    3'b010: ALUCtrl_o = 4'b0111; // SLTI
                    3'b000: ALUCtrl_o = 4'b0010; // ADDI
                    default: ALUCtrl_o = 4'b1111;
                endcase
            end
		default: ALUCtrl_o = 4'b1111;
    endcase
end
endmodule