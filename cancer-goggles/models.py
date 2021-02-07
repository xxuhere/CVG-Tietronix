from time import time
from pathlib import Path
from datetime import datetime

import cv2


class Camera:
    def __init__(self, cam_num, resolution, fourcc, fps, timestamped=False):
        self.cam_num = cam_num
        self.resolution = resolution
        self.fps = fps
        self.timestamped = timestamped

        # using cv2.CAP_DSHOW reduced the fps for single output
        # but it doesn't affect fps for dual outputs
        self.cap = cv2.VideoCapture(self.cam_num, cv2.CAP_DSHOW)
        self.fourcc = cv2.VideoWriter_fourcc(*fourcc)
        self.video_writer = None
        self.last_frame = None

    def initialize_video_writer(self, root_path):
        folder_path = Path(root_path, "video")
        if not folder_path.exists():
            folder_path.mkdir()
        video_path = Path(folder_path, f"record_{int(time())}.avi")
        self.video_writer = cv2.VideoWriter(
            str(video_path), self.fourcc, self.fps, self.resolution
        )

    def is_initialized(self):
        return self.video_writer is not None

    def write(self):
        if self.timestamped:
            now = datetime.now().astimezone().strftime("%A, %d. %B %Y %H:%M:%S %Z")
            frame_copy = self.last_frame.copy()
            cv2.putText(
                frame_copy,
                now,
                (10, 30),
                cv2.FONT_HERSHEY_PLAIN,
                1,
                (210, 155, 155),
                1,
                cv2.LINE_4,
            )
            self.video_writer.write(frame_copy)
        else:
            self.video_writer.write(self.last_frame)

    def get_frame(self):
        if not self.cap.isOpened():
            self.cap.open(self.cam_num)
        ret, video_frame = self.cap.read()
        if video_frame is not None:
            self.last_frame = video_frame
        return self.last_frame

    def snapshot(self, root_path):
        folder_path = Path(root_path, "image")
        if not folder_path.exists():
            folder_path.mkdir()
        snapshot_path = Path(folder_path, f"snapshot_{int(time())}.jpg")
        cv2.imwrite(str(snapshot_path), self.last_frame)

    def close_camera(self):
        if self.video_writer is not None:
            self.video_writer.release()
        self.cap.release()

    def __str__(self):
        return f"OpenCV Camera {self.cam_num}"
