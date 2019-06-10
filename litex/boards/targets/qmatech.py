#!/usr/bin/env python3

import argparse

from migen import *

from litex.boards.platforms import qmatech

from litex.soc.integration.soc_sdram import *
from litex.soc.integration.soc_core import *
from litex.soc.integration.builder import *

from litedram.modules import IS42S16160
from litedram.phy import GENSDRPHY

# CRG ----------------------------------------------------------------------------------------------

class _CRG(Module):
    def __init__(self, platform):
        self.clock_domains.cd_sys = ClockDomain()
        self.clock_domains.cd_sys_ps = ClockDomain()
        self.clock_domains.cd_por = ClockDomain(reset_less=True)

        # # #

        self.cd_sys.clk.attr.add("keep")
        self.cd_sys_ps.clk.attr.add("keep")
        self.cd_por.clk.attr.add("keep")

        # power on rst
        rst_n = Signal()
        self.sync.por += rst_n.eq(1)
        self.comb += [
            self.cd_por.clk.eq(self.cd_sys.clk),
            self.cd_sys.rst.eq(~rst_n),
            self.cd_sys_ps.rst.eq(~rst_n)
        ]

        # sys clk / sdram clk
        clk50 = platform.request("clk50")
        self.comb += self.cd_sys.clk.eq(clk50)
        self.specials += \
            Instance("ALTPLL",
                p_BANDWIDTH_TYPE="AUTO",
                p_CLK0_DIVIDE_BY=1,
                p_CLK0_DUTY_CYCLE=50e0,
                p_CLK0_MULTIPLY_BY=1,
                p_CLK0_PHASE_SHIFT="-3000",
                p_COMPENSATE_CLOCK="CLK0",
                p_INCLK0_INPUT_FREQUENCY=20000e0,
                p_OPERATION_MODE="NORMAL",
                i_INCLK=clk50,
                o_CLK=self.cd_sys_ps.clk,
                i_ARESET=~rst_n,
                i_CLKENA=0x3f,
                i_EXTCLKENA=0xf,
                i_FBIN=1,
                i_PFDENA=1,
                i_PLLENA=1,
            )

        #    Instance("cycloneive_pll",
        #        p_BANDWIDTH_TYPE="AUTO",
        #        p_CLK0_DIVIDE_BY=1,
        #        p_CLK0_DUTY_CYCLE=50e0,
        #        p_CLK0_MULTIPLY_BY=1,
        #        p_CLK0_PHASE_SHIFT="-3000",
        #        p_COMPENSATE_CLOCK="CLK0",
        #        p_INCLK0_INPUT_FREQUENCY=20000e0,
        #        p_OPERATION_MODE="NORMAL",
        #        p_PLL_TYPE="AUTO",
        #        p_LPM_TYPE="cycloneive_pll",
        #        p_CLKSWITCH=0,
        #        p_CONFIGUPDATE=0,
        #        p_PFDENA=1,
        #        p_PHASECOUNTERSELECT=0,
        #        p_PHASESTEP=0,
        #        p_PHASEUPDOWN=0,
        #        p_SCANCLK=0,
        #        p_SCANCLKENA=1,
        #        p_SCANDATA=0,
        #        i_INCLK=clk50,
        #        o_CLK=self.cd_sys_ps.clk,
        #        i_ARESET=~rst_n,
        #    )

        self.comb += platform.request("sdram_clock").eq(self.cd_sys_ps.clk)

# BaseSoC ------------------------------------------------------------------------------------------

class BaseSoC(SoCCore):
    def __init__(self, sys_clk_freq=int(50e6), **kwargs):
        assert sys_clk_freq == int(50e6)
        platform = qmatech.Platform()
        SoCCore.__init__(self, platform, clk_freq=sys_clk_freq,
                         with_uart=True,
                         integrated_rom_size=0x8000,
                         integrated_main_ram_size=0x1000,
                         **kwargs)
        
        self.submodules.crg = _CRG(platform)

# Build --------------------------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(description="LiteX SoC on Qma Tech")
    builder_args(parser)
    soc_sdram_args(parser)
    args = parser.parse_args()

    soc = BaseSoC(**soc_sdram_argdict(args))
    builder = Builder(soc, **builder_argdict(args))
    builder.build()


if __name__ == "__main__":
    main()
