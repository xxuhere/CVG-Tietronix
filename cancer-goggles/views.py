from pathlib import Path
from time import time

from pyqtgraph import ImageView, setConfigOption, PlotItem
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

from meta_dialog import MetaDialog
from parameters import parameters
from parameter_dialog import ParameterDialog


setConfigOption("imageAxisOrder", "row-major")

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
        self.utilization = 0.0
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

        self.image_view = ImageView(view=PlotItem())
        self.image_view.getImageItem().mouseClickEvent = self.image_click
        self.layout.addWidget(self.image_view, 3, 0, 1, 4)

        self.goggles_dispaly = ImageView()
        self.goggles_dispaly.ui.histogram.hide()
        self.goggles_dispaly.ui.roiBtn.hide()
        self.goggles_dispaly.ui.menuBtn.hide()
        self.goggles_dispaly.setWindowFlag(Qt.WindowCloseButtonHint, False)

        self.timer_video = QTimer()
        self.timer_video.timeout.connect(self.update_image)

        self.timer_utilization = QTimer()
        self.timer_utilization.timeout.connect(self.update_utilization_fps)

        self._create_status_bar()

    def _create_status_bar(self):
        self.statusbar = self.statusBar()
        self.statusbar.setContentsMargins(10, 0, 10, 0)

        self.utilization_label = QLabel("Utilization: 0 %")
        self.utilization_label.setContentsMargins(5, 0, 5, 0)
        self.utilization_label.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        self.statusbar.addPermanentWidget(self.utilization_label)

        self.fps_label = QLabel("FPS: 0")
        self.fps_label.setContentsMargins(5, 0, 5, 0)
        self.fps_label.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        self.statusbar.addPermanentWidget(self.fps_label)

        self.user_name_label = QLabel("User Name: test_user")
        self.user_name_label.setContentsMargins(5, 0, 5, 0)
        self.statusbar.addPermanentWidget(self.user_name_label)

        self.patient_name_label = QLabel("Patient ID: test_patient")
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
                f"Patient ID: {meta_info_dialog.patient_name}"
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
        if self.is_recording and not self.camera.is_initialized():
            self.camera.initialize_video_writer(self.root_path)

    def update_image(self):
        start = time()
        frame = self.camera.get_frame()
        if self.is_recording:
            self.camera.write()
        self.image_view.setImage(frame)
        if self.goggles_dispaly.isVisible():
            self.goggles_dispaly.setImage(frame)
        end = time()

        if self.frame_counter == 0:
            self.curr_time = time()

        self.frame_counter += 1

        if self.frame_counter == self.camera.fps:
            self.realtime_fps = self.camera.fps / (time() - self.curr_time)
            self.frame_counter = 0

        # before simplify: (end - start) * ms_per_s / (ms_per_s * self.realtime_fps)
        self.utilization = (end - start) * self.realtime_fps

    def update_utilization_fps(self):
        self.utilization_label.setText(f"Utilization: {self.utilization:.2f} %")
        self.fps_label.setText(f"FPS {self.realtime_fps:.2f}")

    def image_click(self, event):
        x, y = event.pos()
        self.image_view.view.plot([x], [y], symbol="o")
        print(f"clicked ({x}, {y})")

    def start_video(self):
        self.timer_video.start(self.timer_interval)
        self.timer_utilization.start(1000)

    def stop_video(self):
        self.timer_video.stop()
        self.timer_utilization.stop()
        self.utilization_label.setText("Utilization: 0 %")
        self.camera.close_camera()

    def take_snapshot(self):
        self.camera.snapshot(self.root_path)

    def activate_goggles_display(self):
        if self.goggles_dispaly.isVisible():
            self.goggles_dispaly.hide()
        else:
            self.goggles_dispaly.show()

    def closeEvent(self, event):
        msg = "Close the app?"
        reply = QMessageBox.question(
            self, "Message", msg, QMessageBox.Yes, QMessageBox.No
        )

        if reply == QMessageBox.Yes:
            event.accept()
            self.stop_video()
            self.goggles_dispaly.close()
        else:
            event.ignore()
