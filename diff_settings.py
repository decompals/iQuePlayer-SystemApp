def apply(config, args):
    config["baseimg"] = "1106_sa1.bin"
    config["myimg"] = "build/1106_sa1.bin"
    config["mapfile"] = "build/1106_sa1.map"
    config["source_directories"] = ["."]
    # config["show_line_numbers_default"] = True
    # config["arch"] = "mips"
    # config["map_format"] = "gnu" # gnu, mw, ms
    # config["build_dir"] = "build/" # only needed for mw and ms map format
    # config["expected_dir"] = "expected/" # needed for -o
    # config["makeflags"] = []
    # config["objdump_executable"] = ""
