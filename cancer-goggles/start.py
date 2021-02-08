from PyQt5.QtWidgets import QApplication

from models import Camera
from views import StartWindow

# camera_number = "rtsp://10.0.0.7:8555/unicast"
camera_number = 0

resolution = (640, 480)
fourcc = "XVID"
fps = 20
timestamped = True

camera = Camera(camera_number, resolution, fourcc, fps, timestamped=timestamped)

app = QApplication([])
start_window = StartWindow(camera)
start_window.show()
app.exit(app.exec_())
