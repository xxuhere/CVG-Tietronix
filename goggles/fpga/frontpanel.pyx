# cython: language_level=3
# distutils: language=c++
# reference: https://github.com/pkerichang/cyok


from libcpp cimport bool
from libcpp.string cimport string

from goggles.fpga.okFrontPanelDLL cimport (
    okCFrontPanel,
    okCPLL22393,
    okFrontPanelDLL_FreeLib,
    okFrontPanelDLL_GetVersion,
    okFrontPanelDLL_LoadLib,
)


def load_lib(lib_name):
    if lib_name is None:
        success = okFrontPanelDLL_LoadLib(NULL)
    else:
        success = okFrontPanelDLL_LoadLib(lib_name.encode())
    if not success:
        raise RuntimeError("Load FrontPanel DLL failed.")


def free_library():
    okFrontPanelDLL_FreeLib()


def get_version():
    cdef char date[32]
    cdef char time[32]
    okFrontPanelDLL_GetVersion(date, time)
    return date.decode(), time.decode()


cdef class PyOkCPLL22393:
    cdef okCPLL22393 c_22393

    def __init__(self):
        pass

    def set_crystal_load(self, capload):
        self.c_22393.SetCrystalLoad(capload)

    def set_reference(self, freq):
        self.c_22393.SetReference(freq)

    def set_pll_parameters(self, n, p, q):
        self.c_22393.SetPLLParameters(n, p, q)

    def set_output_source(self, n, clksrc):
        self.c_22393.SetOutputSource(n, clksrc)

    def set_output_divider(self, n, div):
        self.c_22393.SetOutputDivider(n, div)

    def set_output_enable(self, n, enable):
        self.c_22393.SetOutputEnable(n, enable)


cdef class PyFrontPanel:
    cdef okCFrontPanel dev
    def __init__(self):
        pass

    @classmethod
    def check_error(cls, int err_code):
        if err_code < 0:
            raise RuntimeError(f'error code = {err_code:d}')

    def set_pll22393_configuration(self, PyOkCPLL22393 pll):
        self.dev.SetPLL22393Configuration(pll.c_22393)

    def get_device_major_version(self):
        return self.dev.GetDeviceMajorVersion()

    def get_device_minor_version(self):
        return self.dev.GetDeviceMinorVersion()

    def get_device_id(self):
        return self.dev.GetDeviceID()

    def get_serial_number(self):
        return self.dev.GetSerialNumber()

    def open_by_serial(self, name=""):
        cdef string name_bytes = name.encode()
        cdef int err_code = self.dev.OpenBySerial(name_bytes)
        self.check_error(err_code)
        return err_code

    def configure_fpga(self, file_name):
        cdef string name_bytes = file_name.encode()
        cdef int err_code = self.dev.ConfigureFPGA(name_bytes)
        self.check_error(err_code)
        return err_code

    def is_front_panel_enabled(self):
        cdef bool enabled = self.dev.IsFrontPanelEnabled()
        return enabled

    def get_wire_in_value(self, int ep_addr):
        cdef unsigned int val
        cdef int err_code = self.dev.GetWireInValue(ep_addr, &val)
        self.check_error(err_code)
        return val

    def get_wire_out_value(self, int ep_addr):
        cdef int val = self.dev.GetWireOutValue(ep_addr)
        return val

    def set_wire_in_value(self, int ep, unsigned int val, unsigned int mask=0xffffffff):
        cdef int err_code = self.dev.SetWireInValue(ep, val, mask)
        return err_code

    def update_wire_ins(self):
        self.dev.UpdateWireIns()

    def update_wire_outs(self):
        self.dev.UpdateWireOuts()

    def read_dual_register(self, reg_address, reg_sensor):
        self.set_wire_in_value(0x06, 0x0001)
        self.set_wire_in_value(0x08,reg_address)
        value = 0xB8B9 if reg_sensor == 1 else 0x9091
        self.set_wire_in_value(0x0B, value)
        self.update_wire_ins()

        while True:
            self.update_wire_outs()
            status = self.get_wire_out_value(0x23)
            if status & 0x0002:
                break

        self.set_wire_in_value(0x06, 0x0002)
        self.update_wire_ins()
        self.update_wire_outs()
        value = self.get_wire_out_value(0x27)
        addr = self.get_wire_out_value(0x28)
        print(f"value: {value}, address: {addr}, sensor {reg_sensor}")

        self.set_wire_in_value(0x06, 0x0000)
        self.update_wire_ins()

    def program_dual_register(self, reg_address, reg_value, reg_sensor):
        self.set_wire_in_value(0x07, 0x0001)
        self.set_wire_in_value(0x08, reg_address)
        self.set_wire_in_value(0x09, reg_value)
        value = 0xB8B9 if reg_sensor == 1 else 0x9091
        self.set_wire_in_value(0x0B, value)
        self.update_wire_ins()

        while True:
            self.update_wire_outs()
            status = self.get_wire_out_value(0x23)
            if status & 0x0001:
                break

        self.set_wire_in_value(0x07, 0x0000)
        self.update_wire_ins()
        self.update_wire_outs()

        value = self.get_wire_out_value(0x23)
        print(f"status: {value}")

        self.set_wire_in_value(0x06, 0x0001)
        self.set_wire_in_value(0x08, reg_address)
        self.update_wire_ins()

        while True:
            self.update_wire_outs()
            status = self.get_wire_out_value(0x23)
            if status & 0x0002:
                break

        print(f"status: {value}")

        self.set_wire_in_value(0x06, 0x0002)
        self.update_wire_ins()
        self.update_wire_outs()

        command_value = self.get_wire_out_value(0x27)
        command_addr = self.get_wire_out_value(0x28)
        print(f"command value: {command_value}, command address: {command_addr}, sensor {reg_sensor}")

        self.set_wire_in_value(0x06, 0x0000)
        self.update_wire_ins()

    def program_stereo_imager(self):
        # broadcast write to de_assert LVDS power_down
        self.program_dual_register(0x00B1, 0x0000, 0)
        self.program_dual_register(0x00B1, 0x0000, 1)

        # individual write to the master clock
        self.program_dual_register(0x00B1, 0x0001, 0)

        # set both sensor to stereoscopic mode
        self.program_dual_register(0x0007, 0x0038, 0)
        self.program_dual_register(0x0007, 0x0038, 1)

        # set pixel operation mode
        self.program_dual_register(0x000F, 0x0002, 0)
        self.program_dual_register(0x000F, 0x0102, 1)

        # set read mode to normal
        self.program_dual_register(0x000D, 0x0300, 0)
        self.program_dual_register(0x000D, 0x0300, 1)

        # set AGC/AEC mode
        self.program_dual_register(0x00AF, 0x0000, 0)
        self.program_dual_register(0x00AF, 0x0000, 1)

        # set desired AGC/AEC bin (brightness level during auto)
        self.program_dual_register(0x00A5, 0x003A, 0)
        self.program_dual_register(0x00A5, 0x003A, 1)

        # set analog gain (manual gain)
        self.program_dual_register(0x0035, 0x0010, 0)
        self.program_dual_register(0x0035, 0x0010, 1)

        # set row noise correction control
        self.program_dual_register(0x0070, 0x0001, 0)
        self.program_dual_register(0x0070, 0x0001, 1)

        # set row noise correction constant
        self.program_dual_register(0x000B, 0x01E0, 0)
        self.program_dual_register(0x000B, 0x01E0, 1)

        self.program_dual_register(0x0003, 0x01E0, 0)
        self.program_dual_register(0x0003, 0x01E0, 1)

        self.program_dual_register(0x0004, 0x02F0, 0)
        self.program_dual_register(0x0004, 0x02F0, 1)

        self.program_dual_register(0x00B3, 0x0000, 0)
        self.program_dual_register(0x00B3, 0x0000, 1)

        self.program_dual_register(0x00B2, 0x0000, 0)
        self.program_dual_register(0x00B2, 0x0000, 1)

        # enable PLL
        self.program_dual_register(0x00B1, 0x0000, 0)
        self.program_dual_register(0x00B1, 0x0000, 1)

        # set the slave sensor to stereo
        self.program_dual_register(0x0007, 0x0078, 1)

        # shift clock out
        self.program_dual_register(0x00B2, 0x0000, 0)
        self.program_dual_register(0x00B2, 0x0000, 1)

        # control ser_data_in
        self.program_dual_register(0x00B3, 0x0000, 0)
        self.program_dual_register(0x00B3, 0x0000, 1)

        self.program_dual_register(0x00B4, 0x0000, 0)
        self.program_dual_register(0x00B4, 0x0000, 1)

        self.program_dual_register(0x00B5, 0x0001, 0)
        self.program_dual_register(0x00B5, 0x0001, 1)

        self.program_dual_register(0x00B7, 0x0007, 0)
        self.program_dual_register(0x00B7, 0x0007, 1)

        self.program_dual_register(0x00B7, 0x0003, 0)
        self.program_dual_register(0x00B7, 0x0003, 1)

        # soft reset
        self.program_dual_register(0x000C, 0x0001, 0)
        self.program_dual_register(0x000C, 0x0001, 1)

        self.program_dual_register(0x000C, 0x0000, 0)
        self.program_dual_register(0x000C, 0x0000, 1)
