from pathlib import Path

from src.fpga.fpga import initialize_fpga

fpga_path = Path(__file__).parent
dll_path = Path(fpga_path, "DLLs", "Win64", "okFrontPanel.dll")
bitfile_path = Path(fpga_path, "bitfile", "goggleimager_flip.bit")
