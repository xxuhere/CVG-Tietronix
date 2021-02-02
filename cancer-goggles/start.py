from PyQt5.QtWidgets import QApplication

from models import Camera
from views import StartWindow

camera_number = 0
resolution = (640, 480)

camera = Camera(camera_number, resolution)
camera.initialize()

app = QApplication([])
start_window = StartWindow(camera)
start_window.show()
app.exit(app.exec_())
