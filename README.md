# Cancer-Goggles

## Dependencies
1. NumPy
2. opencv-python
3. PySide6

## Desktop Setup [outdated]
Install [Miniconda](https://docs.conda.io/en/latest/miniconda.html), and create an environment:
```shell
conda create --name goggles-dev python=3.8 numpy
```

Update conda and activate the environment we just create:
```shell
conda update conda
conda activate goggles-dev
```

opencv-python and PyQt5 are not up to date on conda, so it's better to install them using `pip` at this moment.

```shell
pip install opencv-python pyside6
```

There are many tutorials on Python environment management on the internet. Google if for details.

## Raspberry Pi Setup [outdated]

This App is tested on [Ubuntu Desktop 20.10 64-bit OS](https://ubuntu.com/download/raspberry-pi). 
In general, we should use virtual environments, such as venv and conda, for Python development. 
Currently, there is a bug regarding of PyQt5 installation using pip. So the easiest
way to do this is to use the system's default Python and install PyQt5 using `apt` at the system level.

```shell
sudo apt update
sudo apt upgrade
sudo apt install python3-dev python3-venv python3-pip python3-numpy python3-pyqt5 
```

```shell
python3 -m pip install opencv-python pyqtgraph
```

This procedure is not tested. I'll test it on a fresh installed Pi soon.

## Run the App

```shell
python3 [path to]/start.py
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
