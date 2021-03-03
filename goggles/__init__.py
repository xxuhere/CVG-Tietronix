import os
from pathlib import Path

from PySide6 import __file__ as pyside_path
from PySide6.QtWidgets import QApplication

from goggles.models import Camera
from goggles.views import StartWindow

dirname = Path(pyside_path).parent
plugin_path = Path(dirname, "plugins", "platforms")
os.environ["QT_QPA_PLATFORM_PLUGIN_PATH"] = str(plugin_path)


def start_gui():
    camera_number = 0

    resolution = (640, 480)
    fourcc = "XVID"
    fps = 30
    timestamped = True

    camera = Camera(camera_number, resolution, fourcc, fps, timestamped=timestamped)

    app = QApplication([])
    start_window = StartWindow(camera)
    start_window.show()
    app.exit(app.exec_())
