// Copyright 1986-2017 Xilinx, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2017.3.1 (win64) Build 2035080 Fri Oct 20 14:20:01 MDT 2017
// Date        : Thu Jan 25 01:42:11 2018
// Host        : TERMINATOR running 64-bit Service Pack 1  (build 7601)
// Command     : write_verilog -force -mode synth_stub -rename_top decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix -prefix
//               decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_ block_design_1_ov7670_bram_capture_0_0_stub.v
// Design      : block_design_1_ov7670_bram_capture_0_0
// Purpose     : Stub declaration of top-level module interface
// Device      : xc7z020clg400-1
// --------------------------------------------------------------------------------

// This empty module with port declaration file causes synthesis tools to infer a black box for IP.
// The synthesis directives are for Synopsys Synplify support to prevent IO buffer insertion.
// Please paste the declaration into a Verilog source file or add the file as an additional source.
(* x_core_info = "ov7670_bram_capture,Vivado 2017.3.1" *)
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix(pclk, vsync, href, d, addr, dout, we)
/* synthesis syn_black_box black_box_pad_pin="pclk,vsync,href,d[7:0],addr[17:0],dout[31:0],we" */;
  input pclk;
  input vsync;
  input href;
  input [7:0]d;
  output [17:0]addr;
  output [31:0]dout;
  output we;
endmodule
