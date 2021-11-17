# Cancer-Goggles
[![Code style: black](https://img.shields.io/badge/code%20style-black-000000.svg)](https://github.com/psf/black)
[![Imports: isort](https://img.shields.io/badge/%20imports-isort-%231674b1?style=flat&labelColor=ef8336)](https://pycqa.github.io/isort/)

## DNH Information
For information on the Device Network Hub (DNH), see the DNH folder and the README.md contained in that folder.

## Dashboard Information
For the Dashboard, see the DNH folder and the README_Dashboard.md contained in that folder.

## Desktop Setup
Install [Miniconda](https://docs.conda.io/en/latest/miniconda.html), and update conda base environment:
```shell
conda update conda
```

Create and activate the development environment:
```shell
conda env create --file environment.yml --name goggles-dev
conda activate goggles-dev
```

See the official [documentation](https://docs.conda.io/projects/conda/en/latest/user-guide/tasks/manage-environments.html)
for how to manage conda environments.

Build the Python binding for FPGA and install the App
```shell
python setup.py build_ext
python -m pip install -e .
```

## Raspberry Pi Setup

In general, Qt's support for Arm devices is not great. The only prebuild Qt library on Arm is available via `apt`.
This instruction is for [Ubuntu Desktop 20.10 for Raspberry Pi 4](https://ubuntu.com/download/raspberry-pi).

We chose Ubuntu rather than Raspbian because 64-bit system performs better than 32-bit system with enough resource,
=======
In general, Qt's support for Arm devices is not great. The only prebuild Qt library on Arm is available via `apt`.

This instruction is for [Ubuntu Desktop 20.10 for Raspberry Pi 4](https://ubuntu.com/download/raspberry-pi).

If an image for Ubuntu Desktop 20 for Raspberry Pi cannot be found, use Ubuntu Server 20 LTS for Raspberry Pi (64 bit). Then after installing Ubuntu server, install the GNOME UI on it. Instructions for installing GNOME can be found [here](https://www.cyberciti.biz/faq/ubuntu-linux-install-gnome-desktop-on-server/).

We chose Ubuntu rather than Raspbian because 64-bit system performs better than 32-bit system with enough resource,
and Pi 4 has enough memory take all benefits of a 64-bit OS. Also, the default Python3 version of Ubuntu 20.10
is 3.8, which is identical to the version used in development. So this OS requires the least setup.

The following might not be the optimal solution. But it works for now.

Update packages and install PyQt5
```shell
sudo apt update
sudo apt upgrade
sudo apt install python3-pyqt5
```

Create and activate a virtualenv. See [venv](https://docs.python.org/3/library/venv.html) for details.

install venv:
```shell
sudo apt install python3.8-venv
```

Since we installed pyqt5 at the system level, we need the `--system-site-packages` flag to give venv the access.
```shell
python3 -m venv ~/virtualenvs/goggles-dev --system-site-packages
. ~/virtualenvs/goggles-dev/bin/activate
```

Inside the `goggles-dev` environment, install dependencies
```shell
pip install cython numpy opencv-python pytest
```

Then build the FPGA binding and install the App using the commend from the previous section.

## Test the FPGA
The `--runfpga` flag activates FPGA tests. Those tests would fail if the FPGA is not plugged in.
```shell
pytest --runfpga
```

#### Note: All DLLs are taken from https://github.com/open-ephys/plugin-GUI

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
There are two popular Python Qt bindings: PyQT and PySide. We are currently using PyQt5 5.15.0, which is the latest
version available via `apt` on Ubuntu.

## Alternatives of Qt
[DearPyGui](https://github.com/hoffstadt/DearPyGui) is a new Python Gui framework. It's currently under our evaluation.
There is a demo GUI implemented using this framework. You can check it out with

```shell
python [path to]dearpygui_testy.py
```

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
