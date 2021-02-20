from PySide6.QtCore import Qt
from PySide6.QtWidgets import QGroupBox, QHBoxLayout, QLabel, QSlider


class ParameterItem(QGroupBox):
    def __init__(self, name, low, high, step, default):
        super().__init__()
        layout = QHBoxLayout()

        label_param_name = QLabel(name)
        label_param_name.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        label_param_name.setMinimumWidth(100)
        layout.addWidget(label_param_name)

        param_slider = QSlider(Qt.Horizontal)
        param_slider.setRange(int(low / step), int(high / step))
        param_slider.setFocusPolicy(Qt.NoFocus)
        param_slider.valueChanged.connect(self.change_value)
        layout.addWidget(param_slider)

        label_value = QLabel(str(default))
        label_value.setAlignment(Qt.AlignRight | Qt.AlignVCenter)
        label_value.setMinimumWidth(40)
        layout.addWidget(label_value)
        self.label = label_value

        self.setLayout(layout)

        self.step = step
        self.current_value = default

    def set_value(self, value):
        self.current_value = value

    def change_value(self, value):
        adjusted = value * self.step
        self.label.setText(f"{adjusted:.2f}")
        self.current_value = adjusted
