# Cancer-Goggles
[![Code style: black](https://img.shields.io/badge/code%20style-black-000000.svg)](https://github.com/psf/black)
[![Imports: isort](https://img.shields.io/badge/%20imports-isort-%231674b1?style=flat&labelColor=ef8336)](https://pycqa.github.io/isort/)

## Desktop Setup
Install [Miniconda](https://docs.conda.io/en/latest/miniconda.html), and update conda base environment:
```shell
conda update conda
```

Create and activate the development environment:
```shell
conda create -f environment.yml
conda activate goggles-dev
```

See the official [documentation](https://docs.conda.io/projects/conda/en/latest/user-guide/tasks/manage-environments.html) 
for how to manage conda environments. 


## Build the Python binding for FPGA and install the App
```shell
python setup.py build_ext
python -m pip install -e .
```

## Test the FPGA
The `--runfpga` flag activates FPGA tests. Those tests would fail if the FPGA is not plugged in.
```shell
pytest --runfpga src
```

## Start the GUI
Within `goggles-dev` environment,
```shell
python -m src
```
Or simply,
```shell
goggles
```

## QT Python Bindings
There are two popular Python Qt bindings: PyQT and PySide. We are currently using PySide6, which does not have a pip
or conda wheel up for ARM machine (Raspberry Pi) yet. I'll address this issue soon. 

## Alternatives of Qt
[DearPyGui](https://github.com/hoffstadt/DearPyGui) is a new Python Gui framework. It's currently under our evaluation.

## Streaming

### v4l2rtspserver

Setup v4l2rtspserver on Pi following the [instruction](https://github.com/mpromonet/v4l2rtspserver/wiki/Setup-on-Pi).
Then start the server by (for usb camera)
```shell
v4l2rtspserver -F 25 -H 480 -W 640 -P 8555 /dev/video0
```

Then using a machine on the same network, view the camera streaming using this script:
```python
import cv2

vcap = cv2.VideoCapture("rtsp://[ip]:8555/unicast")
while True:
    ret, frame = vcap.read()
    cv2.imshow('VIDEO', frame)
    cv2.waitKey(1)
```
