`include "common.vh"
`timescale 1ms/10us

// For now, we expose the serial wires txd and rxd as ports to keep ISE from
// optimising away the whole design. Once I figure out what I am doing, these
// should be replaced by whatever actual ports we end up with.
module Tenyr(_halt, _reset, clk, txd, rxd, seg, an);
    wire[31:0] insn_addr, operand_addr;
    wire[31:0] insn_data, in_data, out_data, operand_data;
    wire operand_rw;

    input clk;
    input rxd;
    output txd;

    output[7:0] seg;
    output[3:0] an;

    assign in_data      =  operand_rw ? operand_data : 32'bx;
    assign operand_data = !operand_rw ?     out_data : 32'bz;

    input _reset;
    inout wor _halt;
`ifdef ISIM
    wire downclk = clk;
`else
    tenyr_clk_S clkdiv(.in(clk), .out(downclk));
`endif

    // active on posedge clock
    GenedBlockMem ram(.clka(downclk), .wea(operand_rw), .addra(operand_addr),
                      .dina(in_data), .douta(out_data),
                      .clkb(downclk), .web(1'b0), .addrb(insn_addr),
                      .dinb(32'bx), .doutb(insn_data));

/*
    Serial serial(.clk(downclk), ._reset(_reset), .enable(1'b1), // XXX use halt ?
                  .rw(operand_rw), .addr(operand_addr),
                  .data(operand_data), .txd(txd), .rxd(rxd));
*/

    Seg7 #(.BASE(12'h100))
             seg7(.clk(downclk), ._reset(_reset), .enable(1'b1), // XXX use halt ?
                  .rw(operand_rw), .addr(operand_addr),
                  .data(operand_data), .seg(seg), .an(an));

    Core core(.clk(downclk), ._reset(_reset), .rw(operand_rw),
            .norm_addr(operand_addr), .norm_data(operand_data),
            .insn_addr(insn_addr)   , .insn_data(insn_data), .halt(_halt));
endmodule


