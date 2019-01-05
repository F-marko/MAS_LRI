//Copyright 1986-2018 Xilinx, Inc. All Rights Reserved.
//--------------------------------------------------------------------------------
//Tool Version: Vivado v.2018.2 (win64) Build 2258646 Thu Jun 14 20:03:12 MDT 2018
//Date        : Thu Jan  3 22:21:56 2019
//Host        : TERMINATOR running 64-bit Service Pack 1  (build 7601)
//Command     : generate_target vdma_vtpg_wrapper.bd
//Design      : vdma_vtpg_wrapper
//Purpose     : IP block netlist
//--------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

module vdma_vtpg_wrapper
   (DDR_addr,
    DDR_ba,
    DDR_cas_n,
    DDR_ck_n,
    DDR_ck_p,
    DDR_cke,
    DDR_cs_n,
    DDR_dm,
    DDR_dq,
    DDR_dqs_n,
    DDR_dqs_p,
    DDR_odt,
    DDR_ras_n,
    DDR_reset_n,
    DDR_we_n,
    FIXED_IO_ddr_vrn,
    FIXED_IO_ddr_vrp,
    FIXED_IO_mio,
    FIXED_IO_ps_clk,
    FIXED_IO_ps_porb,
    FIXED_IO_ps_srstb,
    OV7670_D_0,
    OV7670_HREF_0,
    OV7670_PCLK_0,
    OV7670_SCCB_0_scl_io,
    OV7670_SCCB_0_sda_io,
    OV7670_VSYNC_0,
    OV7670_XCLK_0);
  inout [14:0]DDR_addr;
  inout [2:0]DDR_ba;
  inout DDR_cas_n;
  inout DDR_ck_n;
  inout DDR_ck_p;
  inout DDR_cke;
  inout DDR_cs_n;
  inout [3:0]DDR_dm;
  inout [31:0]DDR_dq;
  inout [3:0]DDR_dqs_n;
  inout [3:0]DDR_dqs_p;
  inout DDR_odt;
  inout DDR_ras_n;
  inout DDR_reset_n;
  inout DDR_we_n;
  inout FIXED_IO_ddr_vrn;
  inout FIXED_IO_ddr_vrp;
  inout [53:0]FIXED_IO_mio;
  inout FIXED_IO_ps_clk;
  inout FIXED_IO_ps_porb;
  inout FIXED_IO_ps_srstb;
  input [7:0]OV7670_D_0;
  input OV7670_HREF_0;
  input OV7670_PCLK_0;
  inout OV7670_SCCB_0_scl_io;
  inout OV7670_SCCB_0_sda_io;
  input OV7670_VSYNC_0;
  output OV7670_XCLK_0;

  wire [14:0]DDR_addr;
  wire [2:0]DDR_ba;
  wire DDR_cas_n;
  wire DDR_ck_n;
  wire DDR_ck_p;
  wire DDR_cke;
  wire DDR_cs_n;
  wire [3:0]DDR_dm;
  wire [31:0]DDR_dq;
  wire [3:0]DDR_dqs_n;
  wire [3:0]DDR_dqs_p;
  wire DDR_odt;
  wire DDR_ras_n;
  wire DDR_reset_n;
  wire DDR_we_n;
  wire FIXED_IO_ddr_vrn;
  wire FIXED_IO_ddr_vrp;
  wire [53:0]FIXED_IO_mio;
  wire FIXED_IO_ps_clk;
  wire FIXED_IO_ps_porb;
  wire FIXED_IO_ps_srstb;
  wire [7:0]OV7670_D_0;
  wire OV7670_HREF_0;
  wire OV7670_PCLK_0;
  wire OV7670_SCCB_0_scl_i;
  wire OV7670_SCCB_0_scl_io;
  wire OV7670_SCCB_0_scl_o;
  wire OV7670_SCCB_0_scl_t;
  wire OV7670_SCCB_0_sda_i;
  wire OV7670_SCCB_0_sda_io;
  wire OV7670_SCCB_0_sda_o;
  wire OV7670_SCCB_0_sda_t;
  wire OV7670_VSYNC_0;
  wire OV7670_XCLK_0;

  IOBUF OV7670_SCCB_0_scl_iobuf
       (.I(OV7670_SCCB_0_scl_o),
        .IO(OV7670_SCCB_0_scl_io),
        .O(OV7670_SCCB_0_scl_i),
        .T(OV7670_SCCB_0_scl_t));
  IOBUF OV7670_SCCB_0_sda_iobuf
       (.I(OV7670_SCCB_0_sda_o),
        .IO(OV7670_SCCB_0_sda_io),
        .O(OV7670_SCCB_0_sda_i),
        .T(OV7670_SCCB_0_sda_t));
  vdma_vtpg vdma_vtpg_i
       (.DDR_addr(DDR_addr),
        .DDR_ba(DDR_ba),
        .DDR_cas_n(DDR_cas_n),
        .DDR_ck_n(DDR_ck_n),
        .DDR_ck_p(DDR_ck_p),
        .DDR_cke(DDR_cke),
        .DDR_cs_n(DDR_cs_n),
        .DDR_dm(DDR_dm),
        .DDR_dq(DDR_dq),
        .DDR_dqs_n(DDR_dqs_n),
        .DDR_dqs_p(DDR_dqs_p),
        .DDR_odt(DDR_odt),
        .DDR_ras_n(DDR_ras_n),
        .DDR_reset_n(DDR_reset_n),
        .DDR_we_n(DDR_we_n),
        .FIXED_IO_ddr_vrn(FIXED_IO_ddr_vrn),
        .FIXED_IO_ddr_vrp(FIXED_IO_ddr_vrp),
        .FIXED_IO_mio(FIXED_IO_mio),
        .FIXED_IO_ps_clk(FIXED_IO_ps_clk),
        .FIXED_IO_ps_porb(FIXED_IO_ps_porb),
        .FIXED_IO_ps_srstb(FIXED_IO_ps_srstb),
        .OV7670_D_0(OV7670_D_0),
        .OV7670_HREF_0(OV7670_HREF_0),
        .OV7670_PCLK_0(OV7670_PCLK_0),
        .OV7670_SCCB_0_scl_i(OV7670_SCCB_0_scl_i),
        .OV7670_SCCB_0_scl_o(OV7670_SCCB_0_scl_o),
        .OV7670_SCCB_0_scl_t(OV7670_SCCB_0_scl_t),
        .OV7670_SCCB_0_sda_i(OV7670_SCCB_0_sda_i),
        .OV7670_SCCB_0_sda_o(OV7670_SCCB_0_sda_o),
        .OV7670_SCCB_0_sda_t(OV7670_SCCB_0_sda_t),
        .OV7670_VSYNC_0(OV7670_VSYNC_0),
        .OV7670_XCLK_0(OV7670_XCLK_0));
endmodule
