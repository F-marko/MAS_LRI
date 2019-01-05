// (c) Copyright 1995-2019 Xilinx, Inc. All rights reserved.
// 
// This file contains confidential and proprietary information
// of Xilinx, Inc. and is protected under U.S. and
// international copyright and other intellectual property
// laws.
// 
// DISCLAIMER
// This disclaimer is not a license and does not grant any
// rights to the materials distributed herewith. Except as
// otherwise provided in a valid license issued to you by
// Xilinx, and to the maximum extent permitted by applicable
// law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND
// WITH ALL FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES
// AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY, INCLUDING
// BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-
// INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE; and
// (2) Xilinx shall not be liable (whether in contract or tort,
// including negligence, or under any other theory of
// liability) for any loss or damage of any kind or nature
// related to, arising under or in connection with these
// materials, including for any direct, or any indirect,
// special, incidental, or consequential loss or damage
// (including loss of data, profits, goodwill, or any type of
// loss or damage suffered as a result of any action brought
// by a third party) even if such damage or loss was
// reasonably foreseeable or Xilinx had been advised of the
// possibility of the same.
// 
// CRITICAL APPLICATIONS
// Xilinx products are not designed or intended to be fail-
// safe, or for use in any application requiring fail-safe
// performance, such as life-support or safety devices or
// systems, Class III medical devices, nuclear facilities,
// applications related to the deployment of airbags, or any
// other applications that could lead to death, personal
// injury, or severe property or environmental damage
// (individually and collectively, "Critical
// Applications"). Customer assumes the sole risk and
// liability of any use of Xilinx products in Critical
// Applications, subject only to applicable laws and
// regulations governing limitations on product liability.
// 
// THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS
// PART OF THIS FILE AT ALL TIMES.
// 
// DO NOT MODIFY THIS FILE.


// IP VLNV: xilinx.com:module_ref:ov7670_axi_stream_capture:1.0
// IP Revision: 1

(* X_CORE_INFO = "ov7670_axi_stream_capture,Vivado 2018.2" *)
(* CHECK_LICENSE_TYPE = "vdma_vtpg_ov7670_axi_stream_capture_0_0,ov7670_axi_stream_capture,{}" *)
(* CORE_GENERATION_INFO = "vdma_vtpg_ov7670_axi_stream_capture_0_0,ov7670_axi_stream_capture,{x_ipProduct=Vivado 2018.2,x_ipVendor=xilinx.com,x_ipLibrary=module_ref,x_ipName=ov7670_axi_stream_capture,x_ipVersion=1.0,x_ipCoreRevision=1,x_ipLanguage=VERILOG,x_ipSimLanguage=MIXED}" *)
(* IP_DEFINITION_SOURCE = "module_ref" *)
(* DowngradeIPIdentifiedWarnings = "yes" *)
module vdma_vtpg_ov7670_axi_stream_capture_0_0 (
  pclk,
  vsync,
  href,
  d,
  m_axis_tvalid,
  m_axis_tready,
  m_axis_tlast,
  m_axis_tdata,
  m_axis_tuser,
  aclk
);

input wire pclk;
input wire vsync;
input wire href;
input wire [7 : 0] d;
(* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 m_axis TVALID" *)
output wire m_axis_tvalid;
(* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 m_axis TREADY" *)
input wire m_axis_tready;
(* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 m_axis TLAST" *)
output wire m_axis_tlast;
(* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 m_axis TDATA" *)
output wire [15 : 0] m_axis_tdata;
(* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME m_axis, TDATA_NUM_BYTES 2, TDEST_WIDTH 0, TID_WIDTH 0, TUSER_WIDTH 1, HAS_TREADY 1, HAS_TSTRB 0, HAS_TKEEP 0, HAS_TLAST 1, FREQ_HZ 100000000, PHASE 0.000, CLK_DOMAIN vdma_vtpg_ov7670_axi_stream_capture_0_0_aclk, LAYERED_METADATA undef" *)
(* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 m_axis TUSER" *)
output wire m_axis_tuser;
(* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME aclk, ASSOCIATED_BUSIF m_axis, FREQ_HZ 100000000, PHASE 0.000, CLK_DOMAIN vdma_vtpg_ov7670_axi_stream_capture_0_0_aclk" *)
(* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 aclk CLK" *)
output wire aclk;

  ov7670_axi_stream_capture inst (
    .pclk(pclk),
    .vsync(vsync),
    .href(href),
    .d(d),
    .m_axis_tvalid(m_axis_tvalid),
    .m_axis_tready(m_axis_tready),
    .m_axis_tlast(m_axis_tlast),
    .m_axis_tdata(m_axis_tdata),
    .m_axis_tuser(m_axis_tuser),
    .aclk(aclk)
  );
endmodule
