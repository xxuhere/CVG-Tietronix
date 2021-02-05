from pathlib import Path

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


class StartWindow(QMainWindow):
    def __init__(self, camera=None):
        super().__init__()
        self.setWindowTitle("Cancer Vision")
        self.resize(1280, 720)

        self.camera = camera
        self.root_path = Path(__file__).parent.parent
        self.parameters = parameters
        self.parameters_value = [value for _, _, _, value in parameters.values()]
        self.is_recording = False

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

        self.checkbox_placeholder = QCheckBox("Goggles Display", self.central_widget)
        self.checkbox_placeholder.stateChanged.connect(self.activate_goggles_display)
        self.layout.addWidget(self.checkbox_placeholder, 1, 3)

        self.image_view = ImageView(view=PlotItem())
        self.image_view.getImageItem().mouseClickEvent = self.image_click
        self.layout.addWidget(self.image_view, 3, 0, 1, 4)

        self.goggles_dispaly = ImageView()
        self.goggles_dispaly.ui.histogram.hide()
        self.goggles_dispaly.ui.roiBtn.hide()
        self.goggles_dispaly.ui.menuBtn.hide()

        self.timer_video = QTimer()
        self.timer_video.timeout.connect(self.update_image)

        self._create_status_bar()

    def _create_status_bar(self):
        self.statusbar = self.statusBar()
        self.statusbar.setContentsMargins(10, 0, 10, 0)

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

    def update_image(self):
        frame = self.camera.get_frame()
        if self.is_recording:
            self.camera.write()
        self.image_view.setImage(frame)
        if self.goggles_dispaly.isVisible():
            self.goggles_dispaly.setImage(frame)

    def image_click(self, event):
        x, y = event.pos()
        self.image_view.view.plot([x], [y], symbol="o")
        print(f"clicked ({x}, {y})")

    def start_video(self):
        self.checkbox_record.setDisabled(True)
        if self.is_recording:
            self.camera.initialize_video_writer(self.root_path)
        self.timer_video.start(int(1000 / self.camera.fps))

    def stop_video(self):
        self.timer_video.stop()
        self.camera.close_camera()
        self.checkbox_record.setEnabled(True)

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
        else:
            event.ignore()
