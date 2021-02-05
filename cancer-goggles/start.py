from PyQt5.QtWidgets import QApplication

from models import Camera
from views import StartWindow

camera_number = 0
resolution = (640, 480)
fourcc = "XVID"
fps = 25
timestamped = True

camera = Camera(camera_number, resolution, fourcc, fps, timestamped=timestamped)

app = QApplication([])
start_window = StartWindow(camera)
start_window.show()
app.exit(app.exec_())
