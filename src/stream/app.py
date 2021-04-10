from time import sleep

from flask import Flask, render_template, Response
import cv2

from src.models import Camera

app = Flask(__name__)

camera_number = 0

resolution = (640, 480)
fourcc = "XVID"
fps = 30
timestamped = True

camera = Camera(camera_number, resolution, fourcc, fps, timestamped=timestamped)


def get_frame():
    while True:
        sleep(1/fps)
        frame = camera.get_frame()
        ret, buffer = cv2.imencode('.jpg', frame)
        frame = buffer.tobytes()
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')


@app.route('/video_feed')
def video_feed():
    return Response(get_frame(), mimetype='multipart/x-mixed-replace; boundary=frame')


@app.route('/')
def index():
    """Video streaming home page."""
    return render_template('index.html')


if __name__ == '__main__':
    app.run(debug=True)
