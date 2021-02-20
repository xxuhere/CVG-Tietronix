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

See the official doc for how to manage conda environments https://docs.conda.io/projects/conda/en/latest/user-guide/tasks/manage-environments.html. 


## Build the Python binding for FPGA
```shell
python setup.py build_ext
```

## Test the fpga
The `--runfpga` flag activates tests related to fpga. Those tests would fail if no fpga is plugged in.
```shell
pytest --runfpga goggles
```

## Start the GUI
Within the project root directory, run
```shell
python goggles/start.py
```

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
