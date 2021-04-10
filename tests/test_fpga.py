from pathlib import Path

import pytest

from src.fpga.fpga import initialize_fpga


@pytest.mark.fpga
def test_fpga_initialization():
    base_path = Path("src", "fpga")
    dll_path = Path(base_path, "DLLs", "Win64", "okFrontPanel.dll")
    bitfile_path = Path(base_path, "bitfile", "goggleimager_flip.bit")
    assert dll_path.exists() and bitfile_path.exists()
    initialize_fpga(str(dll_path), str(bitfile_path))
