import pytest

from goggles.fpga.fpga import initialize_fpga


@pytest.mark.fpga
def test_fpga_initialization():
    initialize_fpga("test")
