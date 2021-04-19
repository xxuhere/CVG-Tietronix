from time import time

import cv2
import numpy as np
from PyQt5.QtCore import Qt, QTimer
from PyQt5.QtWidgets import (
    QCheckBox,
    QGridLayout,
    QLabel,
    QPushButton,
    QVBoxLayout,
    QWidget,
)

from src.models import Camera
from src.utils import ndarray_to_qpixmap


class VideoPlayer(QWidget):

    milliseconds_per_seconds = 1000

    def __init__(self, source=None):
        super().__init__()
        self.source = source
        self.video_interval = (
            int(self.milliseconds_per_seconds / self.source.fps)
            if self.source is Camera
            else 0
        )

        layout = QVBoxLayout()

        self.image_view = QLabel()
        self.image_view.setAlignment(Qt.AlignVCenter)
        layout.addWidget(self.image_view)

        self.default_fps_label = "0.00 FPS"
        self.fps_label = QLabel(self.default_fps_label)
        self.fps_label.setContentsMargins(5, 0, 5, 0)
        self.fps_label.setAlignment(Qt.AlignVCenter)
        layout.addWidget(self.fps_label)

        disable = True if self.source is None else False
        self.control_panel = VideoControlPanel(disable)
        layout.addWidget(self.control_panel)

        self.setLayout(layout)

        self.timer_video = QTimer()
        self.timer_video.timeout.connect(self.update_image)

        self.timer_fps = QTimer()
        self.timer_fps.timeout.connect(self.update_fps)

        self.curr_time = 1.0
        self.realtime_fps = 0.0
        self.frame_counter = 0

        w, h = self.source.resolution if self.source is Camera else (640, 480)
        self.black_frame = np.ones((h, w, 3))
        self._set_default_image()

    def start_video(self):
        if not self.source.is_opened():
            return False
        self.timer_video.start(self.video_interval)
        self.timer_fps.start(self.milliseconds_per_seconds)
        return True

    def stop_video(self):
        self.timer_video.stop()
        self.timer_fps.stop()
        self._set_default_image()
        self._reset_fps()

    def snapshot(self):
        self.source.snapshot(self.parent().parent().parent().root_path)

    def update_image(self):
        frame = self.source.get_frame()
        self._set_image(frame)

        if self.control_panel.is_recording:
            self.source.write()

        if self.control_panel.project_to_goggle:
            cv2.imshow("Goggles", frame)

        if self.frame_counter == 0:
            self.curr_time = time()

        self.frame_counter += 1

        if self.frame_counter == self.source.fps:
            self.realtime_fps = self.source.fps / (time() - self.curr_time)
            self.frame_counter = 0

    def update_fps(self):
        self.fps_label.setText(f"{self.realtime_fps:.2f} FPS")

    def video_writer_initialized(self):
        return self.source.is_initialized()

    def initialize_video_writer(self):
        self.source.initialize_video_writer(self.parent().parent().root_path)

    def cleanup(self):
        self.stop_video()
        self.control_panel.cleanup()

    def _reset_fps(self):
        self.fps_label.setText(self.default_fps_label)

    def _set_default_image(self) -> None:
        self._set_image(self.black_frame)

    def _set_image(self, frame: np.ndarray) -> None:
        frame_pixmap = ndarray_to_qpixmap(frame)
        self.image_view.setPixmap(frame_pixmap)


class VideoControlPanel(QWidget):
    def __init__(self, disable=False):
        super().__init__()
        layout = QGridLayout()
        btn_start = QPushButton("Start")
        btn_start.clicked.connect(self.start)
        btn_start.setDisabled(disable)
        layout.addWidget(btn_start, 0, 0)

        btn_stop = QPushButton("Stop")
        btn_stop.clicked.connect(self.stop)
        btn_stop.setDisabled(disable)
        layout.addWidget(btn_stop, 0, 1)

        btn_snap = QPushButton("Snapshot")
        btn_snap.clicked.connect(self.snapshot)
        btn_snap.setDisabled(disable)
        layout.addWidget(btn_snap, 1, 0, 1, 2)

        self.cbx_record = QCheckBox("Record")
        self.cbx_record.stateChanged.connect(self.record)
        self.cbx_record.setDisabled(disable)
        layout.addWidget(self.cbx_record, 2, 0)

        self.cbx_goggle = QCheckBox("To Goggle")
        self.cbx_goggle.stateChanged.connect(self.to_goggle)
        self.cbx_goggle.setDisabled(disable)
        layout.addWidget(self.cbx_goggle, 2, 1)

        self.setLayout(layout)

        self.is_recording = False
        self.project_to_goggle = False

    def start(self):
        self.parent().start_video()

    def stop(self):
        self.parent().stop_video()
        self.cleanup()

    def snapshot(self):
        self.parent().snapshot()

    def record(self, state):
        self.is_recording = True if state == Qt.Checked else False
        if self.is_recording and not self.parent().video_writer_initialized():
            self.parent().initialize_video_writer()

    def to_goggle(self, state):
        self.project_to_goggle = True if state == Qt.Checked else False
        if not self.project_to_goggle:
            cv2.destroyAllWindows()

    def cleanup(self):
        if self.cbx_record.isChecked():
            self.cbx_record.click()
        if self.cbx_goggle.isChecked():
            self.cbx_goggle.click()
        cv2.destroyAllWindows()
