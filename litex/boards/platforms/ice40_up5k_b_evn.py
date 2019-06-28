from litex.build.generic_platform import *
from litex.build.lattice import LatticePlatform

_io = [
    ("rgb_led", 0,
        Subsignal("r", Pins("41")),
        Subsignal("g", Pins("40")),
        Subsignal("b", Pins("39")),
        IOStandard("LVCMOS33")
    ),

    ("user_sw", 0, Pins("23"), IOStandard("LVCMOS33")),
    ("user_sw", 1, Pins("25"), IOStandard("LVCMOS33")),
    ("user_sw", 2, Pins("34"), IOStandard("LVCMOS33")),
    ("user_sw", 3, Pins("43"), IOStandard("LVCMOS33")),

    # The ICE40UP5K-B-EVN does not use the provided FT2232H chip to provide a
    # UART port. One must use their own USB-to-serial cable instead to get a UART.
    # We have chosen to use 48B and 51A for "tx" and "rx" respectively on Header B
    # to implement UART connections. The board comes unpopulated and will need to
    # have headers soldered.
    ("serial", 0,
        Subsignal("tx", Pins("9"), Misc("PULLUP")),
        Subsignal("rx", Pins("6")),
        IOStandard("LVCMOS33")
    ),

    # Only usable in PROG FLASH mode and J7 attached (see PCB silkscreen).
    ("spiflash", 0,
        Subsignal("cs_n", Pins("16"), IOStandard("LVCMOS33")),
        Subsignal("clk", Pins("15"), IOStandard("LVCMOS33")),
        Subsignal("mosi", Pins("14"), IOStandard("LVCMOS33")),
        Subsignal("miso", Pins("17"), IOStandard("LVCMOS33")),
    ),

    ("clk12", 0, Pins("35"), IOStandard("LVCMOS33"))
]

class Platform(LatticePlatform):
    default_clk_name = "clk12"
    default_clk_period = 83.333

    gateware_size = 0x20000

    # FIXME: Create a "spi flash module" object in the same way we have SDRAM
    spiflash_model = "n25q32"
    spiflash_read_dummy_bits = 8
    spiflash_clock_div = 2
    spiflash_total_size = int((32/8)*1024*1024) # 32Mbit
    spiflash_page_size = 256
    spiflash_sector_size = 0x10000

    def __init__(self, device="ice40-up5k", **kwargs):
        LatticePlatform.__init__(self, device + "-sg48", _io, **kwargs)
