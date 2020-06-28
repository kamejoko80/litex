#!/usr/bin/env python3

from migen import *

from litex.soc.interconnect.csr import *
from litex.soc.interconnect import wishbone

SPI_START  = ((8<<8) | (1<<0))
SPI_DONE   = (1<<0)

class Wishbone2SPIDMA(Module, AutoCSR):
    def __init__(self):
        # Wishbone
        self.bus = bus = wishbone.Interface()

        # Control
        self.start = CSR()
        self.done  = CSRStatus()

        # Read parameters: tx source address and length of DMA
        self.tx_src_addr = CSRStorage(32) # DMA TX source address
        self.rx_dst_addr = CSRStorage(32) # DMA RX destination address
        self.tx_len      = CSRStorage(32) # DMA TX size (bytes)
        self.rx_ena      = CSRStorage(32) # DMA RX enable

        # SPI parameters: address of control/status/mosi registers
        self.spi_control_reg_address = CSRStorage(32)
        self.spi_status_reg_address  = CSRStorage(32)
        self.spi_mosi_reg_address    = CSRStorage(32)
        self.spi_miso_reg_address    = CSRStorage(32)

        # # #

        # Shorten CSR's names
        start = self.start.re
        done  = self.done.status

        tx_src_addr = self.tx_src_addr.storage[2:]
        rx_dst_addr = self.rx_dst_addr.storage[2:]
        tx_len      = self.tx_len.storage
        rx_ena      = self.rx_ena

        spi_mosi_reg_address    = self.spi_mosi_reg_address.storage[2:]
        spi_miso_reg_address    = self.spi_miso_reg_address.storage[2:]
        spi_control_reg_address = self.spi_control_reg_address.storage[2:]
        spi_status_reg_address  = self.spi_status_reg_address.storage[2:]

        # internals
        word_offset = Signal(32)
        byte_offset = Signal(3)
        byte_count  = Signal(32)
        tx_data     = Signal(32)
        rx_data     = Signal(32)
        miso_data   = Signal(32)

        # fsm
        self.submodules.fsm = fsm = FSM()
        fsm.act("IDLE",
            If(start,
                NextValue(word_offset, 0),
                NextValue(byte_offset, 0),
                NextValue(byte_count, 0),
                NextValue(rx_data, 0),
                NextState("WISHBONE-READ-TX-DMA-BUFF")
            ).Else(
                done.eq(1),
            )
        )
        fsm.act("WISHBONE-READ-TX-DMA-BUFF",
            bus.stb.eq(1),
            bus.cyc.eq(1),
            bus.adr.eq(tx_src_addr + word_offset),
            If(bus.ack,
                NextValue(tx_data, bus.dat_r),
                NextState("WISHBONE-WRITE-MOSI-REG")
            )
        )
        fsm.act("WISHBONE-WRITE-MOSI-REG",
            bus.stb.eq(1),
            bus.cyc.eq(1),
            bus.we.eq(1),
            bus.adr.eq(spi_mosi_reg_address),
            bus.dat_w.eq(tx_data),
            If(bus.ack,
                NextState("WISHBONE-WRITE-CONTROL-START")
            )
        )
        fsm.act("WISHBONE-WRITE-CONTROL-START",
            bus.stb.eq(1),
            bus.cyc.eq(1),
            bus.we.eq(1),
            bus.adr.eq(spi_control_reg_address),
            bus.dat_w.eq(SPI_START),
            If(bus.ack,
                NextState("SPI-WAIT-DONE")
            )
        )
        fsm.act("SPI-WAIT-DONE",
            bus.stb.eq(1),
            bus.cyc.eq(1),
            bus.adr.eq(spi_status_reg_address),
            If(bus.ack,
                If(bus.dat_r & SPI_DONE,
                    If(rx_ena == 0,
                        NextValue(byte_count, byte_count + 1),
                        NextValue(byte_offset, byte_offset + 1),
                        NextState("SHIFT-BYTE")
                    ).Else(
                        NextValue(byte_count, byte_count + 1),
                        NextValue(byte_offset, byte_offset + 1),
                        NextState("WISHBONE-READ-MISO-REG")
                    )
                )
            )
        )
        fsm.act("WISHBONE-READ-MISO-REG",
            bus.stb.eq(1),
            bus.cyc.eq(1),
            bus.adr.eq(spi_miso_reg_address),
            If(bus.ack,
                NextValue(miso_data, bus.dat_r),
                NextState("SHIFT-BYTE")
            )
        )
        fsm.act("WISHBONE-WRITE-RX-DMA-BUFF-LAST",
            bus.stb.eq(1),
            bus.cyc.eq(1),
            bus.we.eq(1),
            bus.sel.eq(0b1111), # mandatory for writing data to mem
            bus.adr.eq(rx_dst_addr + word_offset),
            bus.dat_w.eq(rx_data),
            If(bus.ack,
                NextState("IDLE")
            )
        )
        fsm.act("WISHBONE-WRITE-RX-DMA-BUFF",
            bus.stb.eq(1),
            bus.cyc.eq(1),
            bus.we.eq(1),
            bus.sel.eq(0b1111), # mandatory for writing data to mem
            bus.adr.eq(rx_dst_addr + word_offset),
            bus.dat_w.eq(rx_data),
            If(bus.ack,
                NextValue(rx_data, 0),
                NextState("INC-WORD-OFFSET")
            )
        )
        fsm.act("SHIFT-BYTE",
            If(byte_count >= tx_len,
                If(rx_ena == 0,
                    NextState("IDLE")
                ).Else(
                    NextValue(rx_data, (rx_data << 8) | miso_data[0:8]),
                    NextState("WISHBONE-WRITE-RX-DMA-BUFF-LAST")
                )
            ).Elif(byte_offset >= 4,
                If(rx_ena == 0,
                    NextValue(byte_offset, 0),
                    NextState("INC-WORD-OFFSET")
                ).Else(
                    NextValue(byte_offset, 0),
                    NextValue(rx_data, (rx_data << 8) | miso_data[0:8]),
                    NextState("WISHBONE-WRITE-RX-DMA-BUFF")
                )
            ).Else(
                NextValue(rx_data, (rx_data << 8) | miso_data[0:8]),
                NextValue(tx_data, tx_data >> 8),
                NextState("WISHBONE-WRITE-MOSI-REG")
            )
        )
        fsm.act("INC-WORD-OFFSET",
            NextValue(word_offset, word_offset + 1),
            NextState("WISHBONE-READ-TX-DMA-BUFF")
        )
