from datetime import datetime
from enum import Enum

import cv2
from PyQt5.QtGui import QImage, QPixmap


class Surgery(Enum):
    Test = 1
    A = 2
    B = 3
    C = 4


def check_cameras() -> (bool, bool):
    cam_0 = cv2.VideoCapture(0)
    cam_0_on, _ = cam_0.read()
    cam_0.release()
    cam_1 = cv2.VideoCapture(1)
    cam_1_on, _ = cam_1.read()
    cam_1.release()
    return cam_0_on, cam_1_on


def ndarray_to_qpixmap(img):
    if img.ndim == 2:
        img = cv2.cvtColor(img, cv2.COLOR_GRAY2RGB)
    w, h, _ = img.shape
    qimg = QImage(img.data, h, w, 3 * h, QImage.Format_RGB888)
    return QPixmap(qimg)


def to_timestamped_frame(img):
    now = datetime.now().astimezone().strftime("%A, %d. %B %Y %H:%M:%S %Z")
    img_copy = img.copy()
    cv2.putText(
        img_copy,
        now,
        (10, 30),
        cv2.FONT_HERSHEY_PLAIN,
        1,
        (210, 155, 155),
        1,
        cv2.LINE_4,
    )
    return img_copy
