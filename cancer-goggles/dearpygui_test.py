import dearpygui.core as dpg
import dearpygui.simple as sdpg
import cv2

from models import Camera


camera_number = 0

resolution = (640, 480)
fourcc = "XVID"
fps = 20
timestamped = True

camera = Camera(camera_number, resolution, fourcc, fps, timestamped=timestamped)

c = 100


def update_canvas(sender, data):
    frame = camera.get_frame()
    frame = cv2.cvtColor(frame, cv2.COLOR_RGB2RGBA)
    dpg.add_texture("texture id", frame, 640, 480)
    dpg.draw_image('drawing', "texture id", [0, 0], [640, 480], uv_min=[0, 0], uv_max=[1, 1])


with sdpg.window("Video Window", width=640, height=480):
    dpg.set_main_window_size(1000, 800)
    dpg.set_main_window_title("Pixel selector")

    dpg.add_drawing('drawing', width=640, height=480)
    dpg.add_label_text("FPS", default_value="0")


dpg.set_render_callback(update_canvas)
dpg.start_dearpygui()
