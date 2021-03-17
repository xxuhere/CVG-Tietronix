from typing import Optional

from src.fpga.frontpanel import PyFrontPanel, PyOkCPLL22393, get_version, load_lib

micron_clock_divider = 70

# those enums are from okFrontPanel source file
# they are nested within c++ class
# therefore it's unclear how to expose them via cython
clock_source_pll_0_0 = 2
clock_source_pll_1_0 = 4
clock_source_pll_2_0 = 6


def initialize_fpga(dll_path: Optional[str], config_file: str):
    load_lib(dll_path)
    date, time = get_version()
    print(f"FrontPanel DLL loaded. Build time: {date}, {time}.")

    dev = PyFrontPanel()
    dev.open_by_serial()

    pll = PyOkCPLL22393()
    pll.set_reference(48.0)
    pll.set_pll_parameters(0, 400, 48)
    pll.set_output_source(0, clock_source_pll_0_0)
    pll.set_output_divider(0, 4)
    pll.set_output_enable(0, True)

    pll.set_pll_parameters(1, 400, 48)
    pll.set_output_source(1, clock_source_pll_1_0)
    pll.set_output_divider(1, micron_clock_divider)
    pll.set_output_enable(1, True)

    pll.set_pll_parameters(2, 400, 48)
    pll.set_output_source(0, clock_source_pll_2_0)
    pll.set_output_divider(0, 3)
    pll.set_output_enable(0, True)

    dev.set_pll22393_configuration(pll)

    print(
        f"Firmware version: {dev.get_device_major_version()}.{dev.get_device_minor_version()}"
    )
    print(f"Serial number: {dev.get_serial_number()}")
    print(f"ID: {dev.get_device_id()}")

    error_code_config = dev.configure_fpga(config_file)
    if error_code_config != 0:
        print(f"FPGA configuration failed, error code: {error_code_config}")

    if not dev.is_front_panel_enabled():
        print("FrontPanel support is not enabled.")
