#!/usr/bin/env python3

import argparse

from migen import *

from litex.boards.platforms import at7core

from litex.soc.cores.clock import *
from litex.soc.integration.soc_core import *
from litex.soc.integration.builder import *

# CRG ----------------------------------------------------------------------------------------------

class _CRG(Module):
    def __init__(self, platform, sys_clk_freq):
        clk50 = platform.request("clk50")
        self.clock_domains.cd_sys = ClockDomain()
        self.clock_domains.cd_clkout = ClockDomain()

        self.comb += [
            self.cd_sys.clk.eq(self.cd_clkout.clk),
        ]

        self.submodules.pll = pll = S7PLL(speedgrade=-2)
        pll.register_clkin(clk50, 50e6)
        pll.create_clkout(self.cd_clkout, sys_clk_freq)

# BaseSoC ------------------------------------------------------------------------------------------

class BaseSoC(SoCCore):
    def __init__(self, sys_clk_freq=int(150e6), **kwargs):
        platform = at7core.Platform()
        SoCCore.__init__(self, platform, clk_freq=sys_clk_freq,
                         with_uart=True,
                         integrated_rom_size=0xA000,   #
                         integrated_sram_size=0x1000,  #
                         integrated_main_ram_size=0,
                         **kwargs)

        self.submodules.crg = _CRG(platform, sys_clk_freq)


# Build --------------------------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(description="LiteX SoC on Artix-7 XC7A100T Core Board")
    builder_args(parser)
    soc_sdram_args(parser)
    args = parser.parse_args()

    soc = BaseSoC(**soc_sdram_argdict(args))
    builder = Builder(soc, **builder_argdict(args))
    builder.build()

if __name__ == "__main__":
    main()
