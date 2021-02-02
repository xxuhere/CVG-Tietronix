from PyQt5.QtWidgets import (
    QHBoxLayout,
    QGroupBox,
    QLabel,
    QSlider,
)
from PyQt5.QtCore import Qt


class ParameterItem(QGroupBox):
    def __init__(self, name, low, high, step, default):
        super().__init__()
        layout = QHBoxLayout()

        layout.addWidget(QLabel(name))

        self.param_slider = QSlider(Qt.Horizontal)
        self.param_slider.setRange(int(low / step), int(high / step))
        self.param_slider.setFocusPolicy(Qt.NoFocus)
        self.param_slider.valueChanged.connect(self.change_value)
        layout.addWidget(self.param_slider)

        self.label = QLabel(str(default), self)
        self.label.setAlignment(Qt.AlignCenter | Qt.AlignVCenter)
        self.label.setMinimumWidth(50)
        layout.addWidget(self.label)

        self.setLayout(layout)

        self.step = step
        self.current_value = default

    def set_value(self, value):
        self.current_value = value

    def change_value(self, value):
        adjusted = value * self.step
        self.label.setText(f"{adjusted:.3f}")
        self.current_value = adjusted
