from enum import Enum

import cv2
from PySide6.QtGui import QPixmap, QImage


class Surgery(Enum):
    Test = 1
    A = 2
    B = 3
    C = 4


def convert_nparray_to_qpixmap(img):
    w, h, ch = img.shape
    if img.ndim == 1:
        img = cv2.cvtColor(img, cv2.COLOR_GRAY2RGB)
    qimg = QImage(img.data, h, w, 3 * h, QImage.Format_RGB888)
    return QPixmap(qimg)
