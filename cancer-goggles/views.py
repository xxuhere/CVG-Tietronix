from pathlib import Path
from time import time

import cv2
from pyqtgraph import ImageView
from PyQt5.QtCore import QTimer, Qt
from PyQt5.QtWidgets import (
    QMainWindow,
    QWidget,
    QPushButton,
    QMessageBox,
    QLabel,
    QFileDialog,
    QGridLayout,
    QCheckBox,
)
import numpy as np

from meta_dialog import MetaDialog
from parameters import parameters
from parameter_dialog import ParameterDialog


class StartWindow(QMainWindow):
    def __init__(self, camera=None):
        super().__init__()
        self.setWindowTitle("Cancer Vision")
        self.resize(1280, 720)

        self.camera = camera
        self.fps = 25
        self.root_path = Path(__file__).parent.parent
        self.folder = "image"
        self.parameters = parameters
        self.parameters_value = [value for _, _, _, value in parameters.values()]
        self.is_recording = False
        self.fourcc = cv2.VideoWriter_fourcc(*"XVID")
        self.video_writer = None

        self.central_widget = QWidget()
        self.setCentralWidget(self.central_widget)
        self.layout = QGridLayout(self.central_widget)

        self.btn_meta_info = QPushButton("Edit Surgery Info", self.central_widget)
        self.btn_meta_info.clicked.connect(self.enter_meta_info)
        self.layout.addWidget(self.btn_meta_info, 0, 0)

        self.btn_directory = QPushButton("Select Main Directory", self.central_widget)
        self.btn_directory.clicked.connect(self.select_directory)
        self.layout.addWidget(self.btn_directory, 0, 1)

        self.btn_parameters = QPushButton("Set Parameters", self.central_widget)
        self.btn_parameters.clicked.connect(self.set_parameters)
        self.layout.addWidget(self.btn_parameters, 0, 2)

        self.checkbox_record = QCheckBox("Recording", self.central_widget)
        self.checkbox_record.stateChanged.connect(self.click_record)
        self.layout.addWidget(self.checkbox_record, 0, 3)

        self.btn_start = QPushButton("Start Video", self.central_widget)
        self.btn_start.clicked.connect(self.start_video)
        self.layout.addWidget(self.btn_start, 1, 0)

        self.btn_stop = QPushButton("Stop Video", self.central_widget)
        self.btn_stop.clicked.connect(self.stop_video)
        self.layout.addWidget(self.btn_stop, 1, 1)

        self.btn_snap = QPushButton("Snapshot", self.central_widget)
        self.btn_snap.clicked.connect(self.take_snapshot)
        self.layout.addWidget(self.btn_snap, 1, 2)

        self.checkbox_placeholder = QCheckBox("Placeholder", self.central_widget)
        # self.checkbox_record.stateChanged.connect(self.click_record)
        self.layout.addWidget(self.checkbox_placeholder, 1, 3)

        self.image_view = ImageView()
        self.layout.addWidget(self.image_view, 3, 0, 1, 4)

        self.timer_video = QTimer()
        self.timer_video.timeout.connect(self.update_image)

        self._create_status_bar()

    def _create_status_bar(self):
        self.statusbar = self.statusBar()
        self.statusbar.setContentsMargins(10, 0, 10, 0)

        self.user_name_label = QLabel("User Name: test_user")
        self.user_name_label.setContentsMargins(5, 0, 5, 0)
        self.statusbar.addPermanentWidget(self.user_name_label)

        self.patient_name_label = QLabel("Patient Name: test_patient")
        self.user_name_label.setContentsMargins(5, 0, 5, 0)
        self.statusbar.addPermanentWidget(self.patient_name_label)

        self.surgery_type_label = QLabel("Surgery Type: test_surgery")
        self.user_name_label.setContentsMargins(5, 0, 5, 0)
        self.statusbar.addPermanentWidget(self.surgery_type_label)

        self.path_label = QLabel(f"Main Directory: {self.root_path}")
        self.user_name_label.setContentsMargins(5, 0, 5, 0)
        self.statusbar.addPermanentWidget(self.path_label)

    def enter_meta_info(self):
        meta_info_dialog = MetaDialog()
        if meta_info_dialog.exec_():
            self.user_name_label.setText(f"User Name: {meta_info_dialog.user_name}")
            self.patient_name_label.setText(
                f"Patient Name: {meta_info_dialog.patient_name}"
            )
            self.surgery_type_label.setText(
                f"Surgery Type: {meta_info_dialog.surgery_type.name}"
            )

    def select_directory(self):
        directory_dialog = QFileDialog()
        directory_dialog.setFileMode(QFileDialog.DirectoryOnly)
        if directory_dialog.exec_():
            directory = directory_dialog.selectedFiles()[0]
            self.path_label.setText(f"Main Directory: {directory}")
            self.root_path = Path(directory)

    def set_parameters(self):
        param = ParameterDialog(parameters)
        if param.exec_():
            self.parameters_value = param.values

    def click_record(self, state):
        self.is_recording = True if state == Qt.Checked else False

    def update_image(self):
        frame = self.camera.get_frame()
        if self.is_recording:
            self.video_writer.write(frame)
        frame = np.swapaxes(frame, 0, 1)
        self.image_view.setImage(frame)

    def start_video(self):
        self.checkbox_record.setDisabled(True)
        if self.is_recording:
            video_folder = Path(self.root_path, "video")
            if not video_folder.exists():
                video_folder.mkdir()
            video_path = Path(video_folder, f"record_{int(time())}.avi")
            self.video_writer = cv2.VideoWriter(
                str(video_path), self.fourcc, self.fps, self.camera.resolution
            )
        self.timer_video.start(int(1000 / self.fps))

    def stop_video(self):
        self.timer_video.stop()
        if self.video_writer is not None:
            self.video_writer.release()
        self.camera.close_camera()
        self.checkbox_record.setEnabled(True)

    def take_snapshot(self):
        self.camera.snapshot(Path(self.root_path, self.folder))

    def closeEvent(self, event):
        msg = "Close the app?"
        reply = QMessageBox.question(
            self, "Message", msg, QMessageBox.Yes, QMessageBox.No
        )

        if reply == QMessageBox.Yes:
            event.accept()
            self.stop_video()
        else:
            event.ignore()
