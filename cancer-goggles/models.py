from time import time
from pathlib import Path

import cv2


class Camera:
    def __init__(self, cam_num, resolution):
        self.cam_num = cam_num
        self.cap = None
        self.resolution = resolution
        self.last_frame = None
        self.fourcc = cv2.VideoWriter_fourcc(*"XVID")

    def initialize(self):
        self.cap = cv2.VideoCapture(self.cam_num)

    def get_frame(self):
        if not self.cap.isOpened():
            self.initialize()
        ret, video_frame = self.cap.read()
        if video_frame is not None:
            self.last_frame = video_frame
        return self.last_frame

    def snapshot(self, folder_path):
        if not folder_path.exists():
            folder_path.mkdir()
        save_path = Path(folder_path, f"snapshot_{int(time())}.jpg")
        cv2.imwrite(str(save_path), self.last_frame)

    def close_camera(self):
        self.cap.release()

    def __str__(self):
        return f"OpenCV Camera {self.cam_num}"
