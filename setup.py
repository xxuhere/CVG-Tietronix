from pathlib import Path

from setuptools import setup
from Cython.Build import cythonize


front_panel = Path("src", "fpga", "frontpanel.pyx")
setup(ext_modules=cythonize(str(front_panel)))
