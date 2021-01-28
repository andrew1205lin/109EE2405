`timescale 1ns / 1ps
module Pipe_CPU(
        clk_i,
		rst_i
		);
    
/****************************************
*               I/O ports               *
****************************************/
input clk_i;
input rst_i;

/****************************************
*            Internal signal            *
****************************************/

/**** IF stage ****/
//control signal...
wire [32-1:0] if_instr_w;
wire [64-1:0] if_pc_addr_w;
wire [64-1:0] mux_pc_result_w;
wire [64-1:0] add1_result_w;
wire [64-1:0] add1_source_w;
assign add1_source_w = 64'd4;//fix #2 forget to add4

/**** ID stage ****/
//control signal...
wire [32-1:0] id_instr_w;
wire [64-1:0] id_pc_addr_w;
wire [64-1:0] id_rf_rs1_data_w;
wire [64-1:0] id_rf_rs2_data_w;
wire [2-1:0]  id_ctrl_alu_op_w;
wire id_ctrl_register_write_w;
wire id_ctrl_branch_w;
wire id_ctrl_alu_mux_w;
wire id_ctrl_mem_write_w;
wire id_ctrl_mem_read_w;
wire id_ctrl_mem_mux_w;
wire [64-1:0]id_Imm_Gen_w; //fix #4 missing 64bit
wire [4:0]id_r1;
reg [4:0]id_r2;
wire [4:0]id_rd;

/**** EX stage ****/
//control signal...
wire ex_alu_zero_w; 
wire [32-1:0] ex_instr_w;//for ALU ctrl
wire [64-1:0] ex_pc_addr_w;//for shifter
wire [64-1:0] ex_rf_rs1_data_w;
wire [64-1:0] ex_rf_rs2_data_w;
wire [2-1:0]  ex_ctrl_alu_op_w;
wire ex_ctrl_register_write_w;
wire ex_ctrl_branch_w;
wire ex_ctrl_alu_mux_w;
wire ex_ctrl_mem_write_w;
wire ex_ctrl_mem_read_w;
wire ex_ctrl_mem_mux_w;
wire [64-1:0]ex_Imm_Gen_w; //fix #4 missing 64bit
wire [4-1:0]  alu_control_w;
wire [64-1:0] mux_alusrc_w;
wire [64-1:0] ex_alu_result_w;
wire [64-1:0] mux3_rf_rs1_data_w;
wire [64-1:0] mux3_rf_rs2_data_w;
wire [64-1:0] ex_add2_sum_w;
wire [64-1:0] shift_left_w;
wire [4:0]ex_r1;
wire [4:0]ex_r2;
wire [4:0]ex_rd;

/**** MEM stage ****/
//control signal...
wire mem_and_result_w;
wire mem_alu_zero_w; 
wire [64-1:0]mem_rf_rs2_data_w;
wire [64-1:0]mem_add2_sum_w;
wire mem_ctrl_register_write_w;
wire mem_ctrl_branch_w;
wire mem_ctrl_mem_write_w;
wire mem_ctrl_mem_read_w;
wire mem_ctrl_mem_mux_w;
wire [64-1:0] mem_alu_result_w;
wire [64-1:0] mem_dataMem_read_w;
wire [4:0]mem_r1;
wire [4:0]mem_r2;
wire [4:0]mem_rd;

/**** WB stage ****/
//control signal...
wire [64-1:0] mux_dataMem_result_w;
wire wb_ctrl_register_write_w;
wire wb_ctrl_mem_mux_w;
wire [64-1:0] wb_dataMem_read_w;
wire [64-1:0] wb_alu_result_w;
wire [4:0]wb_r1;
wire [4:0]wb_r2;
wire [4:0]wb_rd;

/**** Data hazard ****/
//control signal...
wire [1:0]ForwardA; //fix #5
wire [1:0]ForwardB; //fix #5

Forwarding_Unit FU(
		.EX_MEMRegWrite(mem_ctrl_register_write_w),
		.MEM_WBRegWrite(wb_ctrl_register_write_w),
		.EX_MEMRegisterRd(mem_rd),//fix #1, should be like above
		.MEM_WBRegisterRd(wb_rd),//fix #1, should be like above
		.ID_EXRegisterRs1(ex_r1),//fix #8, logic should be like above
		.ID_EXRegisterRs2(ex_r2),//fix #8, logic should be like above
		.ForwardA(ForwardA),
		.ForwardB(ForwardB)	
	);

/****************************************
*          Instantiate modules          *
****************************************/
//Instantiate the components in IF stage
Program_Counter PC(
        .clk_i(clk_i),      
	    .rst_i (rst_i),     
	    .pc_in_i(mux_pc_result_w) ,   
	    .pc_out_o(if_pc_addr_w) 
	);


MUX_2to1 #(.size(64)) Mux_PC_Source(
        .data0_i(add1_result_w),
        .data1_i(mem_add2_sum_w), //from MEM
        .select_i(mem_and_result_w),
        .data_o(mux_pc_result_w)
	);	//haven't connect

Instr_Mem IM(
        .pc_addr_i(if_pc_addr_w),  
	    .instr_o(if_instr_w)    		
	);
			
Adder Add_pc(
        .src1_i(if_pc_addr_w),
	    .src2_i(add1_source_w),     
	    .sum_o(add1_result_w)   
	);

//You need to instantiate many pipe_reg
Pipe_Reg #(.size(32+64)) IF_ID(
        .clk_i(clk_i),      
	    .rst_i (rst_i),
		.data_i({if_pc_addr_w,if_instr_w}),
		.data_o({id_pc_addr_w,id_instr_w})		
	);
		
//Instantiate the components in ID stage
assign id_r1 = id_instr_w[19:15];
always@* begin
	if(id_ctrl_alu_op_w == 2'b10)begin
		id_r2 = id_instr_w[24:20];
	end
	else begin
		id_r2 = 5'b0;
	end
end


assign id_rd = id_instr_w[11:7];


Reg_File RF(
        .clk_i(clk_i),
		.rst_i(rst_i),
		.RS1addr_i(id_instr_w[19:15]) ,
		.RS2addr_i(id_instr_w[24:20]) ,
		.RDaddr_i(wb_rd) ,//notice that here we catch from WB
		.RDdata_i(mux_dataMem_result_w[64-1:0]),//from WB
		.RegWrite_i(wb_ctrl_register_write_w),//notice that here we catch from WB
		.RS1data_o(id_rf_rs1_data_w) ,
		.RS2data_o(id_rf_rs2_data_w)		
	);

Control Control(
        .instr_op_i(id_instr_w[6:0]),
		.Branch_o(id_ctrl_branch_w),
		.MemRead_o(id_ctrl_mem_read_w),
		.MemtoReg_o(id_ctrl_mem_mux_w),
	    .ALU_op_o(id_ctrl_alu_op_w),
		.MemWrite_o(id_ctrl_mem_write_w),
	    .ALUSrc_o(id_ctrl_alu_mux_w),
	    .RegWrite_o(id_ctrl_register_write_w)
	);

Imm_Gen IG(
        .data_i(id_instr_w),
        .data_o(id_Imm_Gen_w)	
	);	

//You need to instantiate many pipe_reg
Pipe_Reg #(.size(8+15+64+64+64+64+32)) ID_EX(//fix #3 lack of 32 bit
        .clk_i(clk_i),      
	    .rst_i (rst_i),
		.data_i({id_ctrl_alu_op_w,id_ctrl_register_write_w,id_ctrl_branch_w,id_ctrl_alu_mux_w,
			id_ctrl_mem_write_w,id_ctrl_mem_read_w,id_ctrl_mem_mux_w,
			id_r1,id_r2,id_rd,
			id_pc_addr_w,id_rf_rs1_data_w,id_rf_rs2_data_w,id_Imm_Gen_w,id_instr_w
			}),
		.data_o({ex_ctrl_alu_op_w,ex_ctrl_register_write_w,ex_ctrl_branch_w,ex_ctrl_alu_mux_w,
			ex_ctrl_mem_write_w,ex_ctrl_mem_read_w,ex_ctrl_mem_mux_w,
			ex_r1,ex_r2,ex_rd,
			ex_pc_addr_w,ex_rf_rs1_data_w,ex_rf_rs2_data_w,ex_Imm_Gen_w,ex_instr_w
			})	
	);
				
//Instantiate the components in EX stage	   
ALU ALU(
        .src1_i(mux3_rf_rs1_data_w),
	    .src2_i(mux_alusrc_w),
	    .ctrl_i(alu_control_w),
	    .result_o(ex_alu_result_w),
		.zero_o(ex_alu_zero_w) // fix #6 missing ex_
	);
		
MUX_3to1 #(.size(64 )) Mux3_1(
        .data0_i(ex_rf_rs1_data_w),
        .data1_i(mux_dataMem_result_w), //from WB
		.data2_i(mem_alu_result_w), //from MEM
        .select_i(ForwardA),
        .data_o(mux3_rf_rs1_data_w)	
    );
		
MUX_3to1 #(.size(64 )) Mux3_2(
        .data0_i(ex_rf_rs2_data_w),
        .data1_i(mux_dataMem_result_w),  //from WB
		.data2_i(mem_alu_result_w),//from MEM
        .select_i(ForwardB),
        .data_o(mux3_rf_rs2_data_w)	
    );
		
ALU_Ctrl AC(
        .funct_i({ex_instr_w[30], ex_instr_w[14:12]}),   
        .ALUOp_i(ex_ctrl_alu_op_w),   
        .ALUCtrl_o(alu_control_w) 	
	);

MUX_2to1 #(.size(64 )) Mux1(
        .data0_i(mux3_rf_rs2_data_w),
        .data1_i(ex_Imm_Gen_w),
        .select_i(ex_ctrl_alu_mux_w),
        .data_o(mux_alusrc_w)
    );
				
Shift_Left_One_64 Shifter(
        .data_i(ex_Imm_Gen_w),
        .data_o(shift_left_w)	
	); 	
		
Adder Add_pc2(
        .src1_i(ex_pc_addr_w),     
	    .src2_i(shift_left_w),     
	    .sum_o(ex_add2_sum_w)  
	);

//You need to instantiate many pipe_reg
Pipe_Reg #(.size(5+64+1+64+15)) EX_MEM(
        .clk_i(clk_i),      
	    .rst_i (rst_i),
		.data_i({ex_ctrl_register_write_w,ex_ctrl_branch_w,
			ex_ctrl_mem_write_w,ex_ctrl_mem_read_w,ex_ctrl_mem_mux_w,
			ex_alu_result_w,ex_alu_zero_w,ex_rf_rs2_data_w,
			ex_r1,ex_r2,ex_rd}),
		.data_o({mem_ctrl_register_write_w,mem_ctrl_branch_w,
			mem_ctrl_mem_write_w,mem_ctrl_mem_read_w,mem_ctrl_mem_mux_w,
			mem_alu_result_w,mem_alu_zero_w,mem_rf_rs2_data_w,
			mem_r1,mem_r2,mem_rd})		
	);	

//Instantiate the components in MEM stage
assign mem_and_result_w = mem_alu_zero_w & mem_ctrl_branch_w;
Data_Mem DM(
        .clk_i(clk_i),      
	    .rst_i (rst_i),
		.addr_i(mem_alu_result_w),
		.data_i(mem_rf_rs2_data_w),
		.MemRead_i(mem_ctrl_mem_read_w),
		.MemWrite_i(mem_ctrl_mem_write_w),
		.data_o(mem_dataMem_read_w)
	);

Pipe_Reg #(.size(2+64+64+15)) MEM_WB(
        .clk_i(clk_i),//fix #7 forget clk rst      
	    .rst_i (rst_i),//fix #7 forget clk rst      	
		.data_i({mem_ctrl_register_write_w,mem_ctrl_mem_mux_w,mem_dataMem_read_w,mem_alu_result_w,mem_r1,mem_r2,mem_rd}),
		.data_o({wb_ctrl_register_write_w,wb_ctrl_mem_mux_w,wb_dataMem_read_w,wb_alu_result_w,wb_r1,wb_r2,wb_rd})	
	);

//Instantiate the components in WB stage
MUX_2to1 #(.size(64)) Mux2(
        .data0_i(wb_alu_result_w),
        .data1_i(wb_dataMem_read_w),
        .select_i(wb_ctrl_mem_mux_w),
        .data_o(mux_dataMem_result_w)	
    );

/****************************************
*           Signal assignment           *
****************************************/
	
endmodule

