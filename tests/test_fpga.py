import pytest

from src.fpga import bitfile_path, dll_path, initialize_fpga


@pytest.mark.fpga
def test_fpga_initialization():
    assert dll_path.exists()
    assert bitfile_path.exists()
    initialize_fpga(str(dll_path), str(bitfile_path))
