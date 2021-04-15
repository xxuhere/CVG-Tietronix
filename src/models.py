from pathlib import Path
from queue import Queue
from threading import Thread
from time import time

import cv2

from src.utils import to_timestamped_frame


class Camera:
    def __init__(self, cam_num, resolution, fourcc, fps, timestamped=False):
        self.cam_num = cam_num
        self.resolution = resolution
        self.fps = fps
        self.timestamped = timestamped

        self.cap = cv2.VideoCapture(self.cam_num)
        self.fourcc = cv2.VideoWriter_fourcc(*fourcc)
        self.video_writer = None
        self.last_frame = None
        self.last_timestamp = None
        self.video_queue = Queue()
        self.video_thread = Thread(target=self._video_writer_worker, daemon=True)

    def is_opened(self):
        return self.cap.isOpened()

    def open(self):
        self.cap.open(self.cam_num)

    def close(self):
        self.__del__()

    def get_frame(self):
        if not self.is_opened():
            self.open()
        ret, video_frame = self.cap.read()
        if ret:
            self.last_frame = video_frame
            self.last_timestamp = int(time())
        return self.last_frame

    def initialize_video_writer(self, root_path):
        folder_path = Path(root_path, "video")
        if not folder_path.exists():
            folder_path.mkdir()
        video_path = Path(folder_path, f"record_{int(time())}.avi")
        self.video_writer = cv2.VideoWriter(
            str(video_path), self.fourcc, self.fps, self.resolution
        )
        self.video_thread.start()

    def is_initialized(self):
        return self.video_writer is not None

    def write(self):
        self.video_queue.put(
            to_timestamped_frame(self.last_frame)
            if self.timestamped
            else self.last_frame
        )

    def _video_writer_worker(self):
        while True:
            frame = self.video_queue.get()
            self.video_writer.write(frame)
            self.video_queue.task_done()

    def snapshot(self, root_path):
        snapshot_thread = Thread(
            target=self._snapshot_thread_function,
            args=(root_path, self.last_frame, self.last_timestamp),
            daemon=True,
        )
        snapshot_thread.run()

    @staticmethod
    def _snapshot_thread_function(root_path, frame, timestamp):
        folder_path = Path(root_path, "image")
        if not folder_path.exists():
            folder_path.mkdir()
        snapshot_path = Path(folder_path, f"{timestamp}.jpg")
        cv2.imwrite(str(snapshot_path), frame)

    def __del__(self):
        self.video_queue.join()
        if self.video_writer is not None:
            self.video_writer.release()
        self.cap.release()

    def __str__(self):
        return f"OpenCV Camera {self.cam_num}"


class DualCamera:
    def __init__(self, resolution, fourcc, fps, timestamped=False):
        self.camera_0 = Camera(0, resolution, fourcc, fps, timestamped)
        self.camera_1 = Camera(1, resolution, fourcc, fps, timestamped)
        self.last_frames = None
        self.last_timestamp = None

    def _check_camera_is_opened(self):
        if not self.camera_0.is_opened():
            self.camera_0.open()
        if not self.camera_1.is_opened():
            self.camera_1.open()

    def get_frames(self):
        self._check_camera_is_opened()
        self.last_frames = (self.camera_0.get_frame(), self.camera_1.get_frame())
        self.last_timestamp = int(time())
