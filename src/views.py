from pathlib import Path
from time import time

import cv2
from PyQt5.QtCore import Qt, QTimer
from PyQt5.QtWidgets import (
    QCheckBox,
    QFileDialog,
    QGridLayout,
    QLabel,
    QMainWindow,
    QMessageBox,
    QPushButton,
    QWidget,
)

from src.meta_dialog import MetaDialog
from src.parameter_dialog import ParameterDialog
from src.parameters import parameters
from src.video_player import VideoPlayer

milliseconds_per_seconds = 1000


class StartWindow(QMainWindow):
    def __init__(self, camera=None):
        super().__init__()
        self.setWindowTitle("Cancer Vision")
        self.resize(1280, 720)

        self.camera = camera
        self.timer_interval = int(milliseconds_per_seconds / self.camera.fps)
        self.root_path = Path(__file__).parent.parent
        self.parameters = parameters
        self.parameters_value = [value for _, _, _, value in parameters.values()]
        self.is_recording = False
        self.goggles_display = False
        self.curr_time = 1.0
        self.realtime_fps = 0.0
        self.frame_counter = 0

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

        self.checkbox_sidebyside = QCheckBox("Side By Side", self.central_widget)
        self.checkbox_sidebyside.stateChanged.connect(self.click_sidebyside)
        self.layout.addWidget(self.checkbox_sidebyside, 0, 4)

        self.btn_start = QPushButton("Start Video", self.central_widget)
        self.btn_start.clicked.connect(self.start_video)
        self.layout.addWidget(self.btn_start, 1, 0)

        self.btn_stop = QPushButton("Stop Video", self.central_widget)
        self.btn_stop.clicked.connect(self.stop_video)
        self.layout.addWidget(self.btn_stop, 1, 1)

        self.btn_snap = QPushButton("Snapshot", self.central_widget)
        self.btn_snap.clicked.connect(self.take_snapshot)
        self.layout.addWidget(self.btn_snap, 1, 2)

        self.checkbox_goggles = QCheckBox("Goggles Display", self.central_widget)
        self.checkbox_goggles.stateChanged.connect(self.activate_goggles_display)
        self.layout.addWidget(self.checkbox_goggles, 1, 3)

        self.checkbox_superimposed = QCheckBox("Superimposed", self.central_widget)
        self.checkbox_superimposed.stateChanged.connect(self.click_superimposed)
        self.layout.addWidget(self.checkbox_superimposed, 1, 4)

        self.image_view_0 = VideoPlayer(self.camera)
        self.layout.addWidget(self.image_view_0, 2, 0, 1, 2)

        self.image_view_1 = VideoPlayer(self.camera)
        self.layout.addWidget(self.image_view_1, 2, 2, 1, 2)

        self.timer_video = QTimer()
        self.timer_video.timeout.connect(self.update_image)

        self.timer_fps = QTimer()
        self.timer_fps.timeout.connect(self.update_fps)

        self._create_status_bar()

    def _create_status_bar(self):
        self.statusbar = self.statusBar()
        self.statusbar.setContentsMargins(10, 0, 10, 0)

        self.user_name_label = QLabel("User Name: test_user")
        self.user_name_label.setContentsMargins(5, 0, 5, 0)
        self.statusbar.addPermanentWidget(self.user_name_label)

        self.patient_name_label = QLabel("Patient ID: test_patient")
        self.patient_name_label.setContentsMargins(5, 0, 5, 0)
        self.statusbar.addPermanentWidget(self.patient_name_label)

        self.surgery_type_label = QLabel("Surgery Type: test_surgery")
        self.surgery_type_label.setContentsMargins(5, 0, 5, 0)
        self.statusbar.addPermanentWidget(self.surgery_type_label)

        self.path_label = QLabel(f"Main Directory: {self.root_path}")
        self.path_label.setContentsMargins(5, 0, 5, 0)
        self.statusbar.addPermanentWidget(self.path_label)

    def enter_meta_info(self):
        meta_info_dialog = MetaDialog()
        if meta_info_dialog.exec_():
            self.user_name_label.setText(f"User Name: {meta_info_dialog.user_name}")
            self.patient_name_label.setText(
                f"Patient ID: {meta_info_dialog.patient_id}"
            )
            self.surgery_type_label.setText(
                f"Surgery Type: {meta_info_dialog.surgery_type.name}"
            )

    def select_directory(self):
        directory_dialog = QFileDialog()
        directory_dialog.setFileMode(QFileDialog.Directory)
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
        if self.is_recording and not self.camera.is_initialized():
            self.camera.initialize_video_writer(self.root_path)

    def click_sidebyside(self, state):
        pass

    def update_image(self):
        frame = self.camera.get_frame()
        if self.is_recording:
            self.camera.write()

        self.image_view_0.set_image(frame)
        self.image_view_1.set_image(frame)

        if self.goggles_display:
            cv2.imshow("Goggles", frame)

        if self.frame_counter == 0:
            self.curr_time = time()

        self.frame_counter += 1

        if self.frame_counter == self.camera.fps:
            self.realtime_fps = self.camera.fps / (time() - self.curr_time)
            self.frame_counter = 0

    def update_fps(self):
        self.image_view_0.set_fps(self.realtime_fps)
        self.image_view_1.set_fps(self.realtime_fps)

    def start_video(self):
        if not self.camera.is_opened():
            msg = "Camera not found."
            QMessageBox.about(self, "Message", msg)
            return
        self.timer_video.start(self.timer_interval)
        self.timer_fps.start(milliseconds_per_seconds)

    def stop_video(self):
        if self.checkbox_record.isChecked():
            self.checkbox_record.click()
        self.timer_video.stop()
        self.timer_fps.stop()
        self.image_view_0.set_fps(0)
        self.image_view_1.set_fps(0)

    def take_snapshot(self):
        self.camera.snapshot(self.root_path)

    def activate_goggles_display(self):
        if self.goggles_display:
            self.goggles_display = False
            cv2.destroyAllWindows()
        else:
            self.goggles_display = True

    def click_superimposed(self, state):
        pass

    def closeEvent(self, event):
        msg = "Close the app?"
        reply = QMessageBox.question(
            self, "Message", msg, QMessageBox.Yes, QMessageBox.No
        )

        if reply == QMessageBox.Yes:
            event.accept()
            cv2.destroyAllWindows()
            self.stop_video()
        else:
            event.ignore()
