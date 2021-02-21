from pathlib import Path

import pytest

from goggles.fpga.fpga import initialize_fpga


@pytest.mark.fpga
def test_fpga_initialization():
    bitfile_path = Path("goggles", "fpga", "goggleimager_flip.bit")
    assert bitfile_path.exists()
    initialize_fpga(str(bitfile_path))
