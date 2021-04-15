import numpy as np
from PyQt5.QtCore import Qt
from PyQt5.QtWidgets import QLabel, QVBoxLayout, QWidget

from src.utils import convert_nparray_to_qpixmap


class VideoPlayer(QWidget):
    def __init__(self, camera=None):
        super().__init__()
        self.camera = camera
        layout = QVBoxLayout()

        self.image_view = QLabel()
        self.image_view.setAlignment(Qt.AlignCenter)
        layout.addWidget(self.image_view)

        self.fps_label = QLabel("0.00 FPS")
        self.fps_label.setContentsMargins(5, 0, 5, 0)
        self.fps_label.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        layout.addWidget(self.fps_label)

        self.setLayout(layout)
        self._set_default_image()

    def _set_default_image(self) -> None:
        h, w = self.camera.resolution
        black_frame = np.ones((w, h, 3))
        self.set_image(black_frame)

    def set_image(self, frame: np.ndarray) -> None:
        frame_pixmap = convert_nparray_to_qpixmap(frame)
        self.image_view.setPixmap(frame_pixmap)

    def set_fps(self, fps: float) -> None:
        self.fps_label.setText(f"{fps:.2f} FPS")
