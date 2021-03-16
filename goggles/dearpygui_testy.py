from threading import Thread
from time import time

import cv2
import numpy as np
from dearpygui.core import (
    add_button,
    add_checkbox,
    add_data,
    add_image,
    add_same_line,
    add_slider_float,
    add_slider_int,
    add_texture,
    get_data,
    set_render_callback,
    set_value,
    start_dearpygui,
)
from dearpygui.simple import group, window

from goggles.models import Camera
from goggles.parameters import parameters

# camera_number = "rtsp://10.0.0.7:8555/unicast"
camera_number = 0

resolution = (640, 480)
fourcc = "XVID"
fps = 20
timestamped = True

camera = Camera(camera_number, resolution, fourcc, fps, timestamped=timestamped)

frame_target = 6
frame_counter = 0
fps_frame_counter = 0
curr_time = 0


def video_callback(sender, data):
    video_on = True if sender == "Start Video" else False
    add_data("video_on", video_on)


def update_canvas(sender, data):

    if not get_data("video_on"):
        return

    global frame_counter, frame_target, fps_frame_counter, curr_time
    if frame_counter == frame_target:
        d = Thread(name="daemon", target=texture_callback, daemon=True)
        d.start()
        frame_counter = 0

        if fps_frame_counter == 0:
            curr_time = time()

        fps_frame_counter += 1

        if fps_frame_counter == fps:
            set_value("realtime_fps", fps / (time() - curr_time))
            fps_frame_counter = 0
    else:
        frame_counter += 1


def texture_callback():
    frame = camera.get_frame()
    frame = cv2.cvtColor(frame, cv2.COLOR_RGB2RGBA)
    add_texture("texture", frame, 640, 480)


with window("Main Window"):
    with group("Left Panel", width=200):
        add_button("Start Video", callback=video_callback)
        add_same_line()
        add_button("End Video", callback=video_callback)
        add_checkbox("Recording", default_value=False, source="is_recording")
        add_same_line(spacing=20)
        add_checkbox("Goggle View", default_value=False, source="display_goggle_view")
        for name, (low, high, step, default) in parameters.items():
            add_slider_int(name, default_value=default, min_value=low, max_value=high, source=name)

    add_same_line()
    image = np.zeros((640, 480, 3), dtype=np.uint8)
    image = cv2.cvtColor(image, cv2.COLOR_RGB2RGBA)
    add_texture("texture", image, 640, 480)
    add_image("canvas", "texture")
    add_slider_float(
        "FPS", max_value=60, enabled=False, no_input=True, source="realtime_fps"
    )

add_data("video_on", False)
set_render_callback(update_canvas)
start_dearpygui(primary_window="Main Window")
